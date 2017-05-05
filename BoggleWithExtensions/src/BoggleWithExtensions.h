/* Name: Kevin Manouchehri Section: Friday 1:30pm, Jean-Luc Watson
 * This is the .h file of the Boggle class. It creates the functions
 * that will allow the human's progress to find a given word to be tracked
 * and for the computer to find all possible words on a Boggle board. Lecture slides,
 * the Stanford CS 106B textbook, and the Stanford C++ Documentation guide were used as aid.
 * EXTRA FEATURES: The Boggle board is now 5x5, and a sound is played after certain
 * scenarios. These scenarios include the user's word being successful, the word being
 * unsuccessful, the board being set up with rattling dice, and two different sounds being
 * played depending on the final score of the game.
 */

#ifndef _boggle_h
#define _boggle_h

#include <iostream>
#include <string>
#include "lexicon.h"
#include "grid.h"
using namespace std;

class Boggle {
public:
    /* Purpose: Sets up a randomly or user-generated initial board and sets up the dictionary.
     * The lexicon dictionary is passed in by reference from the boggle main.
     * This dictionary is copied to be used by the rest of the Boggle class.
     * Board text is a 16-character string of letters or an empty string.
     */
    Boggle(Lexicon& dictionary, string boardText = "", int rowsAndCols = 4);

    /* Purpose: Gets the letter from the Boggle board at the specified row and
     * column and returns that letter. An exception is thrown if the row or column
     * is out of bounds.
     */
    char getLetter(int row, int col);

    /* Purpose: Checks if the user inputted word is long enough, a valid English word,
     * and if the word has not already been used. It returns true if all of those marks are
     * verified and false if any are not.
     */
    bool checkWord(string word);

    /* Purpose: Performs a recursive search on the Boggle board for the word the user has input.
     * It only searches if the word has passed the checkWord check. If the word if found on the
     * board, a point is added to the score. The word parameter passed in is lower case. It returns
     * whether the word was found on the board.
     */
    bool humanWordSearch(string word);

    /* Purpose: Performs a recursive backtracking search function. Returns the set of all words
     * that the computer found in alphabetical order.
     */
    Set<string> computerWordSearch();

    /* Returns the set of words the human has found in a string form.
     */
    string getHumanWords();

    /* Returns the size of the Set of human words.
     */
    int getNumHumanWords();

    /* Returns the score the human has accrued.
     */
    int getHumanScore();

    /* Returns computer's score.
     */
    int getComputerScore();

    /* Returns the number of rows and columns of the Boggle board.
     */
    int getNumRowsAndCols();

    /* Prints the board in a row by column format out to the console.
     */
    friend ostream& operator<<(ostream& out, Boggle& boggle);

private:
    /* Purpose: Searches the board for the input parameter word using recursive backtracking. Chooses
     * a space to search from based on its call to finding available spaces from the current
     * location. It tries to solve from there and backs up if needed. The row and column parameter
     * allows the program to search for available neighbors from that point. It returns true if the
     * word is found.
     */
    bool findHumanWord(string word, int row, int col);

    /* Purpose: Sets the stage for the next function of the recursive computer backtracking. It creates
     * the prefix string and empties out the highlighted word spaces.
     */
    void findComputerWords(Set<string>& result);

    /* Purpose: Performs the computer recursive backtracking function to find
     * the computer to find the words from a given board index. It prunes the search tree
     * by checking if the current word prefix is contained in the dictionary and dives further
     * into that search if so. If not, it undoes the last letter added and goes to the next neighbor
     * index. It returns true when the search has been completed.
     */
    bool findWordsFromBoardIndex(Set<string>& result, string prefix, int row, int col);

    /* Purpose: Finds the neighboring spaces based on the current location on the board. A neighbor
     * is a neighbor is it is within one row and one column of the index. If the row and column indices are -1,
     * the entire board is returned. A grid of the neighboring spaces' row and column index are returned.
     */
    Grid<int> availableSpaces(int row, int col);

    Grid<char> boardGrid;
    Grid<int> highlightedLetters;
    int humanScore, computerScore, numRowsAndCols;
    Lexicon dict;
    Set<string> humanWords;
    string prefixCheck;
};

#endif // _boggle_h
