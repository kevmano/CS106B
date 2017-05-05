/* Name: Kevin Manouchehri Section: Jean-Luc Watson, Friday 1:30pm
 * This program finds a minimum-length ladder between two user-supplied
 * words, which involves changing one letter at a time. Lecture slides and
 * Piazza were as aid in developing this program.
 */

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "lexicon.h"
#include "stack.h"
#include "queue.h"
#include "simpio.h"

using namespace std;

static void welcome();
static void readDictionary(Lexicon &dictionary);
static void printLadder(Stack<string> &hooray);
static void runWordLadder(Lexicon &dictionary, string firstWord, string secondWord);
static void getWords(Lexicon &dictionary);

Stack<string> findNeighbors(string word, Lexicon &dictionary);
Lexicon dictionary;

/* This main creates the welcome message, reads the dictionary file specified by the user,
 * and finds the ladder between the two words specified by the user.
 */
int main() {
    welcome();
    readDictionary(dictionary);
    getWords(dictionary);
    cout << "Have a nice day." << endl;
    return 0;
}

/* This method prints out the welcome message first seen upon running the program.
 */
static void welcome() {
    cout << "Welcome to CS 106B Word Ladder."
         << endl << "Please give me two English words, and I will change the first into the second by changing one letter at a time."
         << endl << endl;
}

/* This method prompts the user for a dictionary file and opens the file upon a successful input.
 */
static void readDictionary(Lexicon &dictionary) {
    ifstream infile;
    dictionary.addWordsFromFile(promptUserForFile(infile, "Dictionary file name? "));
    cout << endl;
}

/* This method asks the user for two words to create a ladder between and reprompts if the words are invalid. The word
 * ladder is then called and run.
 */
static void getWords(Lexicon &dictionary) {
    while (true) {
        string firstWord = getLine("Word #1 (or Enter to quit): ");
        if (firstWord.empty()) break;
        firstWord = toLowerCase(firstWord); // code ignores case
        string secondWord = getLine("Word #2 (or Enter to quit): ");
        if (secondWord.empty()) break;
        secondWord = toLowerCase(secondWord); // code ignores case
        if ((!dictionary.contains(firstWord)) || (!dictionary.contains(secondWord))) {
            cout << "The two words must be found in the dictionary." << endl;
            cout << endl;
        } else if (firstWord == secondWord) {
            cout << "The two words must be different." << endl;
            cout << endl;
        } else if (firstWord.length() != secondWord.length()) {
            cout << "The two words must be the same length." << endl;
            cout << endl;
        } else { // if the words pass all of the checks
            runWordLadder(dictionary, firstWord, secondWord);
        }
    }
}

/* This method creates the word ladder between the two words. It finds a neighbor that is different from the current word by one letter
 * and checks if the neighbor matches the second word the user has input. This method is run until we have a match.
 */
static void runWordLadder(Lexicon &dictionary, string firstWord, string secondWord) {
    Stack<string> stack, hooray;
    Queue<Stack<string>> queue;
    stack.push(firstWord); // initializing the filling of the stack and queue
    queue.enqueue(stack);
    Lexicon neighborPrevUsed;
    while (!queue.isEmpty()) {
        hooray = queue.dequeue();
        string word = hooray.peek();
        Stack<string> neighbors = findNeighbors(word, dictionary);
        while (!neighbors.isEmpty()) {
            string currNeighbor = neighbors.pop();
            if (!neighborPrevUsed.contains(currNeighbor)) {
                if (currNeighbor == secondWord) {
                    hooray.push(secondWord);
                    cout << "A ladder from " << secondWord << " back to " << firstWord << ":" << endl;
                    printLadder(hooray);
                    return;
                } else {
                    Stack<string> copy = hooray;
                    copy.push(currNeighbor); // adds the neighbor to the top of the stack
                    queue.enqueue(copy);
                    neighborPrevUsed.add(currNeighbor);
                }
            }
        }
    }
    cout << "No word ladder found from " << secondWord << " back to " << firstWord << ".";
    cout << endl << endl;
}

/* This creates a stack of strings of the words that differ by one letter of the word at the top
 * of the ladder stack.
 */
Stack<string> findNeighbors(string word, Lexicon &dictionary) {
    Stack<string> stack;
    int size = word.length();
    for (int i = 0; i < size; i++) {
        for (char letter = 'a'; letter <= 'z'; letter++) {
            string modWord = word;
            modWord[i] = letter;
            if (dictionary.contains(modWord) && (modWord != word)) {
                stack.push(modWord);
            }
        }
    }
    return stack;
}

/* This methods prints the successful word ladder to the console beginning with the second word the user
 * specified and leading back to the first.
 */
static void printLadder(Stack<string> &hooray) {
    while (!hooray.isEmpty()) { // reads the stack and adds the top word to the console
        string ladder = hooray.pop();
        cout << ladder << " ";
    }
    cout << endl << endl;
}

