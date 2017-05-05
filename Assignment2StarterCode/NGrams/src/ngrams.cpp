/* Name: Kevin Manouchehri Section: Jean-Luc Watson, Friday 1:30pm
 * This program creates random text based on a document the user enters in. Based on the length of word chains the user
 * specifies, the program determines which words follow groups of N-1 words and creates random sentences from that.
 * Lecture slides and Piazza were used as aid in developing this program.
 */

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "simpio.h"
#include "hashmap.h"
#include "vector.h"

using namespace std;

static void welcome();
static void makeMap();
static void generateWords();
static Vector<string> fillMapKeys(ifstream &infile, int n);
static void accountForWrapAround(Vector<string> firstWords);

HashMap<Vector<string>,Vector<string>> nMap;
Vector<string> window;

/* This main welcomes the user to the program and calls the methods to build the N-grams map and generate the random text.
 */
int main() {
    welcome();
    makeMap();
    generateWords();
    cout << "Exiting." << endl;
    return 0;
}

/* This method prints the welcome message to the console.
 */
static void welcome() {
    cout << "Welcome to CS 106B Random Writer ('N-Grams)."
         << endl << "This program makes random text based on a document."
         << endl << "Give me an input file and an 'N' value for groups of words, and I'll create random text for you."
         << endl << endl;
}

/* This method creates the map of word chains used to generate the random text after prompting the user for the input file
 * and word chain length to use. It maps the Nth word of a chain to the entry containing the N-1 words preceding it.
 */
static void makeMap() {
    ifstream infile;
    promptUserForFile(infile, "Input file? ");
    int n = getInteger("Value of N? ");
    cout << endl;
    Vector<string> firstWords = fillMapKeys(infile, n);
    accountForWrapAround(firstWords);
}

/* This method generates and prints the random strings of words from the document and map. To determine how many words
 * to generate, the user is asked to specify their preference. This method is repeated until the user enters 0 to quit
 * the program.
 */
static void generateWords() {
    while (true) {
    int randWords = getInteger("# of random words to generate (0 to quit)? ");
    if (randWords == 0) return;
    cout << "... ";
    window = nMap.keys()[randomInteger(0, nMap.size()-1)];
    for (int i = 0; i < randWords; i++) {
        cout << window[0] << " ";
        window.add(nMap[window][randomInteger(0, nMap[window].size()-1)]);
        window.remove(0);
    }
    cout << "..." << endl << endl;
    }
}

/* This method return a vector of strings of the first word chain to be used later to account for wrap around. The method also
 * fills the map keys with the word that follows it in the word chain in the user-supplied text.
 */
static Vector<string> fillMapKeys(ifstream &infile, int n) {
    string word;
    while (window.size() < n-1) { // grab those first words for later
        infile >> word;
        window.add(word);
    }
    Vector<string> firstWords = window;
    infile >> word;
    while (!infile.fail()) {
        nMap[window].add(word);
        window.remove(0);
        window.add(word);
        infile >> word;
    }
    return firstWords;
}

/* This method finishes up the creation of the map by accounting for the wrap around from the end of the text to
 * the beginning. This prevents an error from occurring where the final word of the text is selected, and there is
 * no word following it.
 */
static void accountForWrapAround(Vector<string> firstWords) {
    for (int i = 0; i < firstWords.size(); i++) {
        nMap[window].add(firstWords[i]);
        window.remove(0);
        window.add(firstWords[i]);
    }
}
