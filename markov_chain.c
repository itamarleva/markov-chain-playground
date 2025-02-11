#include "markov_chain.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1000


/**
 * Get random number between 0 (includes) and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */

int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain* markov_chain, void* data_ptr)
{
    Node* curr = markov_chain->database->first;
    while (curr != NULL)
    {
        // If the data of the current node matches the given data,
        // return the node
        if (markov_chain->comp_func_ptr(curr->data->data, data_ptr) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

Node* add_to_database(MarkovChain* markov_chain, void* data_ptr)
{
    // Check if the node already exists in the database
    Node* existing_node = get_node_from_database(markov_chain, data_ptr);
    if (existing_node != NULL)
    {
        return existing_node;
    }
    // If the node does not exist, create a new node and add it to the database
    MarkovNode* new_markov_node = malloc(sizeof(MarkovNode));
    if (new_markov_node == NULL)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }
    void* new_data = markov_chain->copy_func_ptr(data_ptr);
    if (new_data == NULL)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        free(new_markov_node);
        new_markov_node = NULL;
        return NULL;
    }
    new_markov_node->data = new_data;
    new_markov_node->frequency_count = 0;
    if (add(markov_chain->database, new_markov_node) == 1)
    {
        markov_chain->free_data_ptr(new_markov_node->data);
        free(new_markov_node);
        return NULL;
    }
    return markov_chain->database->last;
}

int add_node_to_frequency_list(MarkovNode* first_node, MarkovNode* second_node,
                               MarkovChain* markov_chain)
{
    // Check if the second node already exists in
    // the frequency list of the first node
    for (int i = 0; i < first_node->frequency_count; i++)
    {
        if (markov_chain->comp_func_ptr(second_node->data,
                                        first_node->frequency_list[i].markov_node->data) == 0)
        {
            first_node->frequency_list[i].frequency++;
            return EXIT_SUCCESS;
        }
    }
    // If the second node does not exist in the frequency list, add it
    const MarkovNodeFrequency new_freq = {second_node, 1};
    MarkovNodeFrequency* temp;
    if (first_node->frequency_count == 0)
    {
        temp = malloc(sizeof(MarkovNodeFrequency));
    }
    else
    {
        temp = realloc(first_node->frequency_list,
                       (first_node->frequency_count + 1) * sizeof(MarkovNodeFrequency));
    }
    if (temp == NULL)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    first_node->frequency_list = temp;
    first_node->frequency_list[first_node->frequency_count] = new_freq;
    first_node->frequency_count++;
    return EXIT_SUCCESS;
}

void free_markov_chain(MarkovChain** ptr_chain)
{
    // If the database is NULL, there is nothing to free
    if (ptr_chain == NULL || *ptr_chain == NULL)
    {
        return;
    }
    if ((*ptr_chain)->database == NULL)
    {
        free(*ptr_chain);
        *ptr_chain = NULL;
        return;
    }
    Node* curr_index = (*ptr_chain)->database->first;
    while (curr_index != NULL)
    {
        MarkovNode* markov_node = curr_index->data;
        if (markov_node != NULL)
        {
            if (markov_node->data != NULL)
            {
                (*ptr_chain)->free_data_ptr(markov_node->data);
                markov_node->data = NULL;
            }
            if (markov_node->frequency_list != NULL &&
                markov_node->frequency_count > 0)
            {
                free(markov_node->frequency_list);
            }
            markov_node->frequency_list = NULL;
            free(markov_node);
            markov_node = NULL;
        }
        Node* next_node = curr_index->next;
        free(curr_index);
        curr_index = next_node;
    }
    (*ptr_chain)->database = NULL;
    *ptr_chain = NULL;
}


MarkovNode* get_first_random_node(MarkovChain* markov_chain)
{
    while (1)
    {
        const Node* curr_node = markov_chain->database->first;
        const int index = get_random_number(markov_chain->database->size);
        for (int i = 0; i < index; i++)
        {
            curr_node = curr_node->next;
        }
        void* node_data = curr_node->data->data;
        if (!markov_chain->is_last_ptr(node_data))
        {
            return curr_node->data;
        }
    }
}

MarkovNode* get_next_random_node(MarkovNode* cur_markov_node)
{
    int total_words = 0;
    for (int i = 0; i < cur_markov_node->frequency_count; i++)
    {
        total_words += cur_markov_node->frequency_list[i].frequency;
    }
    int word_index = get_random_number(total_words);
    for (int i = 0; i < cur_markov_node->frequency_count; i++)
    {
        word_index -= cur_markov_node->frequency_list[i].frequency;
        if (word_index < 0)
        {
            return cur_markov_node->frequency_list[i].markov_node;
        }
    }
    return NULL;
}

void generate_random_sequence(MarkovChain* markov_chain,
                              MarkovNode* first_node, int max_length)
{
    MarkovNode* curr_node = first_node;
    markov_chain->print_func_ptr(curr_node->data);
    printf(" ");
    for (int i = 0; i < max_length - 1; i++)
    {
        MarkovNode* next_node = get_next_random_node(curr_node);
        void* next_word = next_node->data;
        markov_chain->print_func_ptr(next_word);
        if (markov_chain->is_last_ptr(next_word))
        {
            printf("\n");
            return;
        }
        printf(" ");
        curr_node = next_node;
    }
    printf("\n");
}
