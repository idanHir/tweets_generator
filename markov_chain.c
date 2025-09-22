#include "markov_chain.h"


bool add_new_node_to_frequency_list(MarkovNode *first_node,
                                    MarkovNode *second_node)
{
  if(first_node->freq_list_act_size < first_node->freq_list_full_size)
  {
    first_node->frequencies_list[first_node->freq_list_act_size]
    .next_object = second_node;
    first_node->frequencies_list[first_node->freq_list_act_size]
        .frequency = 1;
    first_node->freq_list_act_size += 1;
    return true;
  }
  // In this case we don't have enough memory, and need to reallocate.
  MarkovNodeFrequency *temp =
      realloc (first_node->frequencies_list, (1 +
      first_node->freq_list_act_size) * sizeof(MarkovNodeFrequency));
  if(!temp)
  {
    free(first_node->frequencies_list);
    first_node->frequencies_list = NULL;
    return false;
  }
  first_node->frequencies_list = temp;
  first_node->frequencies_list[first_node->freq_list_act_size]
  .next_object = second_node;
  first_node->frequencies_list[first_node->freq_list_act_size]
      .frequency = 1;
  first_node->freq_list_act_size += 1;
  first_node->freq_list_full_size += 1;
  return true;
}

Node *get_node_from_database (MarkovChain *markov_chain, void
*data_ptr)
{
  Node *curr = markov_chain->database->first;
  while(curr)
  {
    void *curr_s = curr->data->data;
    if(markov_chain->comp_func(curr_s, data_ptr) == 0)
    {
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}


Node *add_to_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *curr_node = get_node_from_database (markov_chain, data_ptr);
  if(!curr_node){
    MarkovNode *new_mark_node = malloc (sizeof (MarkovNode));
    if(!new_mark_node)
    {
      return NULL;
    }
    new_mark_node->data = markov_chain->copy_func(data_ptr);
    new_mark_node->frequencies_list = NULL;
    new_mark_node->freq_list_act_size = 0;
    new_mark_node->freq_list_full_size = 0;
    if (add (markov_chain->database, new_mark_node) != 0)
    {
      // Roll back allocation to avoid leak on add() failure
      markov_chain->free_data(new_mark_node->data);
      new_mark_node->data = NULL;
      free(new_mark_node);
      return NULL;
    }
    return get_node_from_database (markov_chain, data_ptr);
  }
  return curr_node;
}

bool add_node_to_frequencies_list (MarkovNode *first_node,
                                   MarkovNode *second_node,
                                   MarkovChain *markov_chain)
{
  MarkovNodeFrequency *freq_lst = first_node->frequencies_list;
  // Adds 1 to the frequency parameter if second node exists in the
  // list.
  for(size_t i=0; i < first_node->freq_list_act_size; i++)
  {
    if(markov_chain->comp_func(freq_lst[i].next_object->data,
        second_node->data)==0)
    {
      freq_lst[i].frequency += 1;
      return true;
    }
  }
  // otherwise, add the second node into the first one's frequency
  // list.
  if(add_new_node_to_frequency_list (first_node, second_node))
  {
    return true;
  }
  return false;
}

void free_database (MarkovChain **ptr_chain)
{
  // changed to last, cause apparently it goes from last to first...
  Node *curr = (*ptr_chain)->database->first;
  while(curr)
  {
    MarkovNode *curr_mark = curr->data;
    (*ptr_chain)->free_data(curr_mark->data);
    curr_mark->data = NULL;
    free(curr_mark->frequencies_list);
    curr_mark->frequencies_list = NULL;
    free(curr_mark);
    curr_mark = NULL;
    Node *temp_next = curr->next;
    free(curr);
    curr = NULL;
    curr = temp_next;
  }
  free((*ptr_chain)->database);
  ((*ptr_chain)->database) = NULL;
  free((*ptr_chain));
  *ptr_chain = NULL;
}
int get_random_number (int max_number)
{
  return rand() % max_number;
}

Node *get_first_random_node_helper (MarkovChain *markov_chain,
                              int* ind, Node *curr_node)
{
  while(markov_chain->is_last(curr_node->data->data) ||
  curr_node->data->frequencies_list == NULL)
  {
    *ind = get_random_number (markov_chain->database->size);
    curr_node = markov_chain->database->first;
    for(int i=0; i < *ind; i++)
    {
      curr_node = curr_node->next;
    }
  }
  return curr_node;
}

MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  int ind = get_random_number (markov_chain->database->size);
  Node *curr_node = markov_chain->database->first;
  for(int i=0; i < ind; i++)
  {
    curr_node = curr_node->next;
  }
  curr_node = get_first_random_node_helper (markov_chain, &ind,
                                         curr_node);
  return curr_node->data;
}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  int total_freq = 0;
  for (size_t i = 0; i < state_struct_ptr->freq_list_act_size; i++)
  {
    total_freq += state_struct_ptr->frequencies_list[i].frequency;
  }
  MarkovNode **mark_arr = malloc(total_freq * sizeof(MarkovNode *));
  if (mark_arr == NULL)
  {
    fprintf(stdout, "Allocation failure: can't allocate an array.");
    return NULL;
  }
  int k = 0;
  for (size_t i = 0; i < state_struct_ptr->freq_list_act_size; i++) {
    MarkovNodeFrequency freq = state_struct_ptr->frequencies_list[i];
    for (int j = 0; j < freq.frequency; j++) {
      mark_arr[k] = freq.next_object;
      k++;
    }
  }
  int rand_ind = get_random_number (total_freq);
  MarkovNode *chosen_node = mark_arr[rand_ind];
  free(mark_arr);
  mark_arr = NULL;
  return chosen_node;
}


void generate_tweet (MarkovChain *markov_chain, MarkovNode
  *first_node, int max_length)
/** First we'll create an array to hold pointers to chars, which we'll
 * receive from the Markov nodes. The array will be used to store
 * the tweet. **/
{
  int arr_len = 1;
  void* tweet[MAX_TWEET_LEN] = {first_node->data};
  MarkovNode *curr_node = first_node;
  for(int i=1; i<max_length; i++)
  {
    curr_node = get_next_random_node (curr_node);
    tweet[i] = curr_node->data;
    if(markov_chain->is_last(curr_node->data) ||
    curr_node->frequencies_list == NULL)
    {
      arr_len = i;
      break;
    }
    arr_len = i;
  }
  for(int j=0; j<arr_len; j++)
  {
    markov_chain->print_func(tweet[j]);
    markov_chain->print_func(" ");
  }
  markov_chain->print_func(tweet[arr_len]);
  if(arr_len == MAX_TWEET_LEN-1)
  {
    markov_chain->print_func(".");
  }
  markov_chain->print_func("\n");
}
