#include <string.h>
#include "linked_list.h"
#include "markov_chain.h"
#define SEED_ARG 1
#define TWEET_COUNT_ARG 2
#define FILE_PATH_ARG 3
#define MIN_ARG_COUNT 3
#define MAX_ARG_COUNT 4
#define WORDS_TO_READ_ARG 4
#define DECIMAL 10
#define MAX_WORD_LEN 100
#define MAX_TWEET_LEN 20
#define MAX_LINE_LEN 1000

char *strcpy_modified(void *word_to_copy)
{
  char *new_word = malloc(MAX_WORD_LEN);
  strcpy (new_word, word_to_copy);
  return new_word;
}


bool is_last_test(void *test_arr)
{
  char *test_word = (char *) test_arr;
  if(test_word[strlen(test_word)-1] == '.')
  {
    return true;
  }
  return false;
}

static int create_new_freq_list (MarkovNode *markov_node)
{
  markov_node->frequencies_list = malloc (sizeof
                                              (MarkovNodeFrequency));
  if (markov_node->frequencies_list == NULL)
  {
    fprintf (stdout, "Allocation failure: can't allocate a freq list");
    return EXIT_FAILURE;
  }
  markov_node->freq_list_full_size = 1;
  return EXIT_SUCCESS;
}

static int fill_database_helper (MarkovChain *markov_chain,
                          char *first_word, char *second_word)
{
  Node *curr_node = add_to_database (markov_chain, first_word);
  if (curr_node == NULL)
  {
    fprintf (stdout, "Allocation failure: couldn't allocate a node.");
    free_database (&markov_chain);
    return 1;
  }
  if (markov_chain->is_last(curr_node->data->data))
  {
    return 0;
  }
  Node *next_node = add_to_database (markov_chain, second_word);
  if (next_node == NULL)
  {
    fprintf (stdout, "Allocation failure: couldn't allocate a node.");
    free_database (&markov_chain);
    return 1;
  }
  MarkovNode *second_node = next_node->data;
  if (curr_node->data->frequencies_list == NULL)
  {
    create_new_freq_list (curr_node->data);
  }
  if (add_node_to_frequencies_list (curr_node->data, second_node,
                                    markov_chain) == false)
  {
    fprintf (stdout, "Allocation failure: couldn't allocate a node.");
    free_database (&markov_chain);
    return 1;
  }
  return 0;
}

void print_str(void *data)
{
  char *converted_st = (char *) data;
  while(*converted_st != '\0')
  {
    printf("%c", *converted_st);
    converted_st++;
  }
}

static void find_file_len (FILE *f, int *file_size)
{
  fseek (f, 0, SEEK_END);
  *file_size = (int) ftell (f);
  rewind (f);
}

static int
fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  if (fp == NULL)
  {
    return 1;
  }
  int file_size, word_counter = 0;
  find_file_len (fp, &file_size);
  char sentence[MAX_LINE_LEN];
  while (fgets (sentence, file_size, fp) != NULL)
  {
    char *first_word = strtok (sentence, " \n\r");
    char *second_word = strtok (NULL, " \n\r");
    word_counter += 1;
    if (first_word == NULL)
    {
      return 0;
    }
    while (first_word != NULL)
    {
      if (fill_database_helper (markov_chain, first_word,
                                second_word) == 1)
      {
        free_database (&markov_chain);
        return 1;
      }
      word_counter += 1;
      first_word = second_word;
      second_word = strtok (NULL, " \n\r");
      if (word_counter >= words_to_read)
      {
        return 0;
      }
    }
  }
  return 0;
}

int set_chain_attributes(MarkovChain *main_chain)
{
  main_chain->comp_func = (comp_function) &strcmp;
  main_chain->copy_func = (copy_function) &strcpy_modified;
  main_chain->free_data = (free_function) &free;
  main_chain->is_last = (is_last_function) &is_last_test;
  main_chain->print_func = (print_function) &print_str;
  main_chain->database = malloc (sizeof (LinkedList));
  if(main_chain->database == NULL)
  {
    free(main_chain);
    fprintf (stdout, "Allocation failure: couldn't allocate a "
                     "database.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/** ownership of the markov chain is the scope's, don't need to
later on free the markov chain itself, only it's database! **/
int main (int argc, char *argv[])
{
  if (argc != MIN_ARG_COUNT + 1 && argc != MAX_ARG_COUNT + 1)
  {
    fprintf (stdout, "Usage: invalid parameters. Seed, tweet count,"
                     " valid file path and optionally read count "
                     "need to be submitted.\n");
    return EXIT_FAILURE;
  }
  unsigned int seed = (int) strtol (argv[SEED_ARG], NULL, DECIMAL);
  srand (seed);
  int tweet_count = (int) strtol (argv[TWEET_COUNT_ARG], NULL,
                                  DECIMAL);
  FILE *input = fopen (argv[FILE_PATH_ARG], "r");
  if (input == NULL)
  {
    fprintf (stdout, "Error: file can't be opened\n");
    return EXIT_FAILURE;
  }
  int read_count = 0;
  find_file_len (input, &read_count);
  if (argc == MAX_ARG_COUNT + 1)
  {
    read_count = (int) strtol (argv[WORDS_TO_READ_ARG], NULL,
                               DECIMAL);
  }
  MarkovChain* main_chain = malloc (sizeof(MarkovChain));
  if(main_chain == NULL)
  {
    fprintf (stdout, "Allocation failure: can't allocate chain.");
    return EXIT_FAILURE;
  }
  if(set_chain_attributes (main_chain) == EXIT_FAILURE)
  {
   return EXIT_FAILURE;
  }
  fill_database (input, read_count, main_chain);
  MarkovNode *first_node = get_first_random_node (main_chain);
  for (int i = 1; i <= tweet_count; i++)
  {
    printf ("%s %d: ", "Tweet", i);
    generate_tweet (main_chain, first_node, MAX_TWEET_LEN);
    if (i == tweet_count)
    {
      break;
    }
    first_node = get_first_random_node (main_chain);
  }
  free_database (&main_chain);
  fclose (input);
  return EXIT_SUCCESS;
}

//  print_frequencies (main_chain.database);
//  print_list (main_chain.db);
//  printf("%d", main_chain.db->size);
