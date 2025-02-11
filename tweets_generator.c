#include <limits.h>
#include "markov_chain.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"
#define BUFFER_SIZE 1000
#define MAX_WORDS_IN_TWEET 20
#define DECIMAL 10
#define ARGS_WITH_OPTIONAL 5
#define ARGS_WITHOUT_OPTIONAL 4
#define DELIMITERS " \n\t\r"

void print_word(void* word)
{
    char* char_word = word;
    printf("%s", char_word);
}

int comp_words(void* first_word, void* second_word)
{
    const char* first_char = first_word;
    const char* second_char = second_word;
    return strcmp(first_char, second_char);
}

void free_word(void* word)
{
    free(word);
}

void* copy_word(void* word)
{
    char* copy = malloc(strlen(word) + 1);
    if (copy == NULL)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }
    strcpy(copy, word);
    return copy;
}

bool end_with_dot(void* word)
{
    const char* char_word = word;
    const int len = strlen(char_word);
    if (char_word[len - 1] == '.')
    {
        return true;
    }
    return false;
}

// Function declaration
int fill_database(FILE* fp, int words_to_read, MarkovChain* markov_chain);

// Main function
int main(const int argc, char* argv[])
{
    if (argc < ARGS_WITHOUT_OPTIONAL || argc > ARGS_WITH_OPTIONAL)
    {
        printf(NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    const unsigned int seed = strtol(argv[1], NULL, DECIMAL);
    srand(seed);
    const unsigned int tweets_number = strtol(argv[2], NULL, DECIMAL);
    const char* file_path = argv[3];
    int words_number = INT_MAX;
    if (argc == ARGS_WITH_OPTIONAL)
    {
        words_number = strtol(argv[4], NULL, DECIMAL);
    }
    FILE* file = fopen(file_path, "r");
    if (file == NULL)
    {
        printf(FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }
    LinkedList link_list = {NULL, NULL, 0};
    MarkovChain markov_chain = {
        &link_list, print_word, comp_words,
        free_word, copy_word, end_with_dot
    };
    MarkovChain* markov_chain_ptr = &markov_chain;
    if (fill_database(file, words_number, &markov_chain) == EXIT_FAILURE)
    {
        fclose(file);
        return EXIT_FAILURE;
    }
    fclose(file);
    for (unsigned int i = 1; i <= tweets_number; i++)
    {
        MarkovNode* first_word_node = get_first_random_node(&markov_chain);
        printf("Tweet %d: ", i);
        generate_random_sequence(markov_chain_ptr,
                                 first_word_node, MAX_WORDS_IN_TWEET);
    }
    free_markov_chain(&markov_chain_ptr);
    return EXIT_SUCCESS;
}

// Function to fill the database with words from the file
int fill_database(FILE* fp, int words_to_read, MarkovChain* markov_chain)
{
    char buffer[BUFFER_SIZE] = {0};
    char* token = NULL;
    const Node* prev_node = NULL;
    // Read lines from file
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL && words_to_read > 0)
    {
        token = strtok(buffer, DELIMITERS); // Tokenize line
        while (token != NULL)
        {
            // Add token to database
            const Node* added_node = add_to_database(markov_chain, token);
            if (added_node == NULL)
            {
                free_markov_chain(&markov_chain);
                markov_chain = NULL;
                return EXIT_FAILURE;
            }
            // Add node to frequency list if it's not the end of a sentence
            if (prev_node != NULL)
            {
                const char* word = prev_node->data->data;
                if (!(markov_chain->is_last_ptr((void*)word)))
                {
                    if (add_node_to_frequency_list(prev_node->data,
                                                   added_node->data, markov_chain) == EXIT_FAILURE)
                    {
                        free_markov_chain(&markov_chain);
                        return EXIT_FAILURE;
                    }
                }
            }
            prev_node = added_node;
            words_to_read--;
            // If it's the end of a sentence, reset prev_node
            const char* word = prev_node->data->data;
            if (markov_chain->is_last_ptr((void*)word))
            {
                if (words_to_read <= 0)
                {
                    return EXIT_SUCCESS;
                }
                prev_node = NULL;
            }
            token = strtok(NULL, DELIMITERS); // Get next token
        }
    }
    return EXIT_SUCCESS;
}
