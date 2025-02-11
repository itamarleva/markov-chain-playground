#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))
#define ARGS_NUM 3
#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define DECIMAL 10
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {
    {13, 4},
    {85, 17},
    {95, 67},
    {97, 58},
    {66, 89},
    {87, 31},
    {57, 83},
    {91, 25},
    {28, 50},
    {35, 11},
    {8, 30},
    {41, 62},
    {81, 43},
    {69, 32},
    {20, 39},
    {33, 70},
    {79, 99},
    {23, 76},
    {15, 47},
    {61, 14}
};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell
{
    int number; // Cell number 1-100
    int ladder_to; // ladder_to represents the jump of the
    //ladder in case there is one from this square
    int snake_to; // snake_to represents the jump of the
    //snake in case there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
int handle_error_snakes(char* error_msg, MarkovChain** database)
{
    printf("%s", error_msg);
    if (database != NULL)
    {
        free_markov_chain(database);
    }
    return EXIT_FAILURE;
}


int create_board(Cell* cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(cells[j]);
            }
            handle_error_snakes(ALLOCATION_ERROR_MASSAGE,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell){i + 1, EMPTY, EMPTY};
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
int fill_database_snakes(MarkovChain* markov_chain)
{
    Cell* cells[BOARD_SIZE];
    if (create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain, cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain,
                                             cells[index_to])->data;
            add_node_to_frequency_list(from_node,
                                       to_node, markov_chain);
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*)(from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain,
                                                 cells[index_to])->data;
                int res = add_node_to_frequency_list(from_node,
                                                     to_node, markov_chain);
                if (res == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
        }
    }
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

void print_cell(void* cell)
{
    const Cell* cell_ptr = cell;
    const int cell_num = cell_ptr->number;
    printf("[%d] ", cell_num);
    if (cell_ptr->ladder_to != EMPTY)
    {
        printf("-ladder to");
    }
    else if (cell_ptr->snake_to != EMPTY)
    {
        printf("-snake to");
    }
    if (cell_num != BOARD_SIZE)
    {
        printf("->");
    }
}

int comp_cells(void* first_cell, void* second_cell)
{
    const Cell* first_cell_ptr = first_cell;
    const Cell* second_cell_ptr = second_cell;
    return first_cell_ptr->number - second_cell_ptr->number;
}

void free_cell(void* cell)
{
    free(cell);
}

void* copy_cell(void* cell)
{
    Cell* dest = malloc(sizeof(Cell));
    if (dest == NULL)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }
    const Cell* src = cell;
    *dest = *src;
    return dest;
}

bool is_last_cell(void* cell)
{
    const Cell* cell_ptr = cell;
    if (cell_ptr->number == BOARD_SIZE)
    {
        return true;
    }
    return false;
}


/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char* argv[])
{
    if (argc != ARGS_NUM)
    {
        printf(NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    const unsigned int seed = strtol(argv[1], NULL, DECIMAL);
    srand(seed);
    const unsigned int path_num = strtol(argv[2], NULL, DECIMAL);
    LinkedList link_list = {NULL, NULL, 0};
    MarkovChain markov_chain = {
        &link_list, print_cell, comp_cells,
        free_cell, copy_cell, is_last_cell
    };
    MarkovChain* markov_chain_ptr = &markov_chain;
    Cell* cells[BOARD_SIZE];
    if (create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (fill_database_snakes(markov_chain_ptr) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode* first_node = link_list.first->data;
    for (unsigned int i = 1; i <= path_num; i++)
    {
        printf("Random Walk %d: ", i);
        generate_random_sequence(markov_chain_ptr,
                                 first_node, MAX_GENERATION_LENGTH);
    }
    free_markov_chain(&markov_chain_ptr);
    return EXIT_SUCCESS;
}
