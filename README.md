**Markov Chain Playground:**
Markov Chain Playground is a C-based framework that demonstrates the power of generic Markov chains through two fun applications: a tweet generator and a Snakes & Ladders simulator.
---

**Overview:**
This project provides a generic implementation of a Markov chain in C, using a linked list to store states and transitions. It showcases how Markov chains can be used to generate random sequences, whether for creating tweet-like sentences or simulating random walks in a Snakes & Ladders game.

---

**Features**
Generic Markov Chain Implementation:
Easily add, search, and traverse nodes with customizable data handling (copy, compare, free, etc.).

Tweets Generator:
Reads a text file and generates random tweet sequences based on the frequency of word transitions.

Snakes & Ladders Simulator:
Simulates game moves on a board with snakes and ladders using a Markov chain to determine random walks.

---

**Project Structure:**
CMakeLists.txt:
Basic CMake configuration for the project.

Makefile:
Build targets for compiling the tweet generator and the Snakes & Ladders simulator.

markov_chain.c / markov_chain.h:
Core Markov chain functionalities, including sequence generation and memory management.

linked_list.c / linked_list.h:
Implementation of a generic linked list used for the Markov chain database.

tweets_generator.c:
Contains the logic for generating tweets from a text file.

snakes_and_ladders.c:
Implements the Snakes & Ladders game simulation using a Markov chain.

justdoit_tweets.txt:
Sample text file with tweet content for testing the tweet generator.

---

**Requirements:**
A C compiler (e.g., GCC)
GNU Make
CMake (optional, if you prefer a CMake-based build)

---

**Building the Project:**
Using the Makefile:
To build the Tweets Generator:
make tweets_generator

To build the Snakes & Ladders Simulator:
make snakes_and_ladders

To clean up the compiled binaries:
make clean

Using CMake:
(Optional) If you prefer CMake, you can build the project as follows:
mkdir build
cd build
cmake ..
make

---

**Running the Applications:**
Tweets Generator:
Usage:
./tweets_generator <seed> <tweets_number> <file_path> [words_number]

seed: A numeric seed for the random number generator.
tweets_number: The number of tweets to generate.
file_path: Path to the text file containing tweet data.
words_number (optional): Maximum number of words to read from the file (defaults to reading the entire file).

Example:
./tweets_generator 1234 5 justdoit_tweets.txt 1000

Snakes & Ladders Simulator:
Usage:
./snakes_and_ladders <seed> <paths_number>

seed: A numeric seed for the random number generator.
paths_number: The number of random walk simulations (game paths) to generate.

Example:
./snakes_and_ladders 5678 3

---

**How It Works:**
The project uses a generic linked list to store Markov nodes. Each node encapsulates data and a frequency list that tracks transitions to subsequent nodes. For the tweet generator, words are parsed from an input file and linked based on their occurrence sequence. In the Snakes & Ladders simulator, each board cell (with potential snakes or ladders) is treated as a state, and transitions mimic dice rolls and game mechanics.

The Markov chain enables generating random sequences by:

Selecting an initial state at random.
Choosing the next state based on the weighted frequency of transitions.
Continuing until a designated "end" state is reached or a maximum length is achieved.

---

**Contributing:**
Contributions, bug reports, and suggestions are welcome! Feel free to fork the repository and submit pull requests with improvements or additional features.

---

Happy coding and enjoy exploring the Markov Chain Playground!
