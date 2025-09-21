# Markov Chain Applications

This project implements two applications using Markov chains: a **Tweets Generator** and a **Snakes and Ladders Simulator**. Both applications utilize the same underlying Markov chain logic to generate sequences based on probabilistic transitions between states.

## Project Purpose

### 1. Tweets Generator
The tweets generator uses Markov chains to create realistic-looking tweets by analyzing word patterns from input text files. It builds a probabilistic model where each word is a state, and the transitions represent the likelihood of one word following another. The generator then creates new tweets by randomly walking through this chain, producing text that mimics the style and patterns of the input data.

### 2. Snakes and Ladders Simulator
The snakes and ladders simulator models the classic board game using Markov chains. Each cell on the board represents a state, and the transitions represent possible moves (dice rolls, ladders, and snakes). The simulator generates random game sequences showing the path a player would take from start to finish, including all ladder climbs and snake slides.

## File Descriptions

### Core Implementation Files

- **`markov_chain.h`** - Header file defining the Markov chain data structures and function prototypes. Contains the `MarkovChain` and `MarkovNode` structures, along with function pointers for generic operations (print, compare, copy, free, is_last).

- **`markov_chain.c`** - Implementation of the Markov chain functionality including:
  - Database management (adding nodes, retrieving nodes)
  - Frequency list management for transition probabilities
  - Random node selection and sequence generation
  - Memory management and cleanup functions

- **`linked_list.h`** - Header file defining the linked list data structure used to store Markov nodes in the database.

- **`linked_list.c`** - Implementation of the linked list operations, providing the underlying data structure for the Markov chain database.

### Application Files

- **`tweets_generator.c`** - Main application for generating tweets using Markov chains. Features:
  - Parses text files to build word transition models
  - Generates multiple tweets based on learned patterns
  - Configurable parameters for seed, tweet count, and word limit
  - Handles sentence endings (words ending with '.')

- **`snakes_and_ladders.c`** - Main application for simulating Snakes and Ladders games. Features:
  - Creates a 100-cell board with predefined ladders and snakes
  - Models dice rolls (1-6) and special transitions
  - Generates complete game sequences from start to finish
  - Shows the path including all ladder climbs and snake slides

### Build and Configuration Files

- **`makefile`** - Build configuration with two targets:
  - `tweets`: Compiles the tweets generator
  - `snake`: Compiles the snakes and ladders simulator

- **`CMakeLists.txt`** - CMake configuration for building the project

- **`justdoit_tweets.txt`** - Sample input file containing tweets for the generator to learn from

## How to Run

### Compilation

Use the provided makefile to compile the applications:

```bash
# Compile the tweets generator
make tweets

# Compile the snakes and ladders simulator
make snake
```

Alternatively, you can compile manually:
```bash
# Tweets generator
gcc -Wall -Wvla markov_chain.c tweets_generator.c linked_list.c -o tweets_generator

# Snakes and ladders simulator
gcc -Wall -Wvla snakes_and_ladders.c markov_chain.c linked_list.c -o snakes_and_ladders
```

### Running the Applications

#### Tweets Generator
```bash
./tweets_generator <seed> <tweet_count> <file_path> [words_to_read]
```

**Parameters:**
- `seed`: Random seed for reproducible output
- `tweet_count`: Number of tweets to generate
- `file_path`: Path to input text file
- `words_to_read`: (Optional) Maximum number of words to read from file

**Example:**
```bash
./tweets_generator 42 5 justdoit_tweets.txt
```

#### Snakes and Ladders Simulator
```bash
./snakes_and_ladders <seed> <game_count>
```

**Parameters:**
- `seed`: Random seed for reproducible output
- `game_count`: Number of games to simulate

**Example:**
```bash
./snakes_and_ladders 123 3
```

### Sample Output

**Tweets Generator:**
```
Tweet 1: Just do it! Believe in yourself and never give up.
Tweet 2: Success comes to those who work hard and stay focused.
```

**Snakes and Ladders:**
```
Random Walk 1: [1] -> [2] -> [3] -> [4] -ladder to 30 -> [30] -> [31] -> [32] -> [33] -ladder to 70 -> [70] -> [71] -> [72] -> [73] -> [74] -> [75] -> [76] -> [77] -> [78] -> [79] -ladder to 99 -> [99] -> [100]
```

## Technical Details

- **Language**: C
- **Memory Management**: Manual memory allocation with proper cleanup
- **Data Structures**: Linked lists for Markov chain database, dynamic arrays for frequency lists
- **Random Generation**: Uses `srand()` and `rand()` for reproducible random sequences
- **Error Handling**: Comprehensive error checking for memory allocation and file operations