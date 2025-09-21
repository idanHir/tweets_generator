tweets: markov_chain.c markov_chain.h tweets_generator.c linked_list.c linked_list.h
	gcc -Wall -Wvla markov_chain.c tweets_generator.c linked_list.c -o tweets_generator

snake: snakes_and_ladders.c markov_chain.c markov_chain.h linked_list.c linked_list.h
	gcc -Wall -Wvla snakes_and_ladders.c markov_chain.c linked_list.c -o snakes_and_ladders