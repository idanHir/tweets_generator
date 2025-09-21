#include "linked_list.h"
#include "markov_chain.h"

int add(LinkedList *link_list, void *data)
{
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return 1;
    }
    *new_node = (Node) {data, NULL};

    if (link_list->first == NULL)
    {
        link_list->first = new_node;
        link_list->last = new_node;
    }
    else
    {
        link_list->last->next = new_node;
        link_list->last = new_node;
    }

    link_list->size++;
    return 0;
}

void print_frequencies (LinkedList *link_list)
{ //TODO: delete later
  Node *curr_node = link_list->first;
  while (curr_node != NULL) {
    MarkovNode *curr_mark_node = curr_node->data;
    MarkovNodeFrequency *curr_freq = curr_mark_node->frequencies_list;
    printf("%s ", (char *) curr_mark_node->data);
    for(int i=0; i < curr_mark_node->freq_list_act_size; i++) {
      if(i>0)
      {
        if(i+1 == curr_mark_node->freq_list_act_size)
        {
          printf("'%s' (%d)", (char *) curr_freq[i].next_object->data,
                 curr_freq[i].frequency);
          continue;
        }
        printf("'%s' (%d), ", (char *) curr_freq[i].next_object->data,
               curr_freq[i].frequency);
        continue;
      }
      printf("-> '%s' (%d) ", (char *) curr_freq[i].next_object->data,
             curr_freq[i].frequency);
    }
    printf("\n");
    curr_node = curr_node->next;
  }
}
