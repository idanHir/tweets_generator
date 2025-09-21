cmake_minimum_required(VERSION 3.16)
project(ex3b-idan_hirsch C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(ex3b-idan_hirsch
        justdoit_tweets.txt
        linked_list.c
        linked_list.h
        markov_chain.h
        markov_chain.c tweets_generator.c snakes_and_ladders.c)
