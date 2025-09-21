#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"
#include <stddef.h>

#define SEED_ARG 1
#define TWEET_COUNT_ARG 2
#define ARG_COUNT 2

#define DECIMAL 10

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY (-1)
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell
{
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of
    // the ladder in case there is one from this square
    int snake_to;  // snake_to represents the jump of
    // the snake in case there is one from this square
    //both ladder_to and snake_to should be
    // -1 if the Cell doesn't have them
} Cell;

static bool is_last_cell (Cell *suspect_cell)
{
  if (suspect_cell->number == BOARD_SIZE)
  {
    return true;
  }
  return false;
}

static Cell *copy_cell (Cell *org_cell)
{
  Cell *new_cell = malloc (sizeof (Cell));
  if(!new_cell)
  {
    printf("Allocation failure: couldn't copy a cell.");
    return NULL;
  }
  memcpy (new_cell, org_cell, sizeof (Cell));
  return new_cell;
}

int compare_cells (Cell *first_cell, Cell *second_cell)
{
  int first_cell_coord = first_cell->number;
  int second_cell_coord = second_cell->number;
  return first_cell_coord - second_cell_coord;
}

void print_cell (void *data)
{
  Cell *cell_to_check = (Cell *) data;
  if (cell_to_check->ladder_to != EMPTY)
  {
    printf ("%s %d", "-ladder to", cell_to_check->ladder_to);
  }
  if (cell_to_check->snake_to != EMPTY)
  {
    printf ("%s %d", "-snake to", cell_to_check->snake_to);
  }
}

/** Error handler **/
static int handle_error (char *error_msg, MarkovChain **database)
{
  printf ("%s", error_msg);
  if (database != NULL)
  {
    free_database (database);
  }
  return EXIT_FAILURE;
}

static int create_board (Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    cells[i] = malloc (sizeof (Cell));
    if (cells[i] == NULL)
    {
      for (int j = 0; j < i; j++)
      {
        free (cells[j]);
      }
      handle_error (ALLOCATION_ERROR_MASSAGE, NULL);
      return EXIT_FAILURE;
    }
    *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
  }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
  {
    int from = transitions[i][0];
    int to = transitions[i][1];
    if (from < to)
    {
      cells[from - 1]->ladder_to = to;
    }
    else
    {
      cells[from - 1]->snake_to = to;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database (MarkovChain *markov_chain)
{
  Cell *cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database (markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database (markov_chain, cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
      to_node = get_node_from_database (markov_chain, cells[index_to])
          ->data;
      add_node_to_frequencies_list (from_node, to_node, markov_chain);
    }
    else
    {
      for (int j = 1; j <= DICE_MAX; j++)
      {
        index_to = ((Cell *) (from_node->data))->number + j - 1;
        if (index_to >= BOARD_SIZE)
        {
          break;
        }
        to_node = get_node_from_database (markov_chain, cells[index_to])
            ->data;
        add_node_to_frequencies_list (from_node, to_node, markov_chain);
      }
    }
  }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free (cells[i]);
  }
  return EXIT_SUCCESS;
}

Cell **generate_game (MarkovChain *main_chain, int max_length, size_t
*track_len)
{
  MarkovNode *curr_mark_node = (MarkovNode *) main_chain
      ->database->first->data;
  Cell **cell_track = malloc (sizeof (Cell *) * max_length);
  if(cell_track == NULL)
  {
    printf("Allocation failure: couldn't set an array of cells.");
    return NULL;
  }
  cell_track[0] = (Cell *) curr_mark_node->data;
  for (int i = 1; i < max_length; i++)
  {
    curr_mark_node = get_next_random_node (curr_mark_node);
    cell_track[i] = (Cell *) curr_mark_node->data;
    *track_len += 1;
    if (main_chain->is_last (curr_mark_node->data) ||
        curr_mark_node->frequencies_list == NULL)
    {
      break;
    }
  }
  return cell_track;
}

int set_snake_chain_attributes(MarkovChain *main_chain)
{
  main_chain->copy_func = (copy_function) &copy_cell;
  main_chain->print_func = (print_function) &print_cell;
  main_chain->is_last = (is_last_function) &is_last_cell;
  main_chain->comp_func = (comp_function) &compare_cells;
  main_chain->free_data = (free_function) &free;
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

int print_tracks(MarkovChain *main_chain, int
amount_of_games_to_generate)
{
  size_t track_len = 0;
  for (int i = 1; i <= amount_of_games_to_generate; i++)
  {
    printf ("%s %d%s ", "Random Walk", i, ":");
    Cell **cell_track = generate_game
        (main_chain, MAX_GENERATION_LENGTH, &track_len);
    if(cell_track == NULL)
    {
      return EXIT_FAILURE;
    }
    for(size_t j=0; j <= track_len; j++)
    {
      printf ("%s%d%s", "[", cell_track[j]->number, "]");
      main_chain->print_func (cell_track[j]);
      if(!(j == track_len && main_chain->is_last(cell_track[j])))
      {
        printf(" %s ", "->");
      }
    }
    printf ("%c", '\n');
    track_len = 0;
    free(cell_track);
  }
  return EXIT_SUCCESS;
}
/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main (int argc, char *argv[])
{
  unsigned int seed = (int) strtol (argv[SEED_ARG], NULL, DECIMAL);
  srand (seed);
  int amount_of_games_to_generate = (int) strtol
      (argv[TWEET_COUNT_ARG], NULL, DECIMAL);
  if (argc != ARG_COUNT + 1)
  {
    return EXIT_FAILURE;
  }
  MarkovChain* main_chain = malloc ((sizeof (MarkovChain)));
  if(main_chain == NULL)
  {
    printf("Allocation failure: couldn't create the Markov chain.");
    return EXIT_FAILURE;
  }
  if(set_snake_chain_attributes (main_chain) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  fill_database (main_chain);
  if(print_tracks(main_chain, amount_of_games_to_generate) ==
  EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  free_database (&main_chain);
  return EXIT_SUCCESS;
}
