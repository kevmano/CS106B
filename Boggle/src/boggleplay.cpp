/* Name: Kevin Manouchehri Section: Friday 1:30pm, Jean-Luc Watson
 * This program plays the game of Boggle and manages the user interaction
 * and GUI and console output of the results of the game. It utilizes the
 * Boggle class to run the game. Lecture slides, the Stanford CS 106B textbook,
 * and the Stanford C++ Documentation guide were used as aid.
 */

#include "lexicon.h"
#include "Boggle.h"
#include "bogglegui.h"
#include "simpio.h"
#include "console.h"
#include <cctype>

void initializeOrReset(int rowsAndCols);
Boggle setUpBoard(Lexicon &dictionary, int rowsAndCols);
bool isValidUserBoard(string &userBoardText);
void humanTurn(Boggle &board);
void computerTurn(Boggle &board);
void printResultOfGame(Boggle &board);
void printHumanStatus(Boggle &board);
void printHumanWordMessage(bool validWord, string humanWord);
void printMessageToConsoleAndGUI(string message);

/* This manages one game of Boggle. It sets up the Boggle board and console
 * to interact with, plays the human's turn, plays the computer's turn, and then
 * prints out the result of the game.
 */
void playOneGame(Lexicon& dictionary) {
    int rowsAndCols = 4;
    initializeOrReset(rowsAndCols);
    Boggle board = setUpBoard(dictionary, rowsAndCols);
    clearConsole();
    for (int i = 0; i < rowsAndCols; i++) {
        for (int j = 0; j < rowsAndCols; j++) {
            BoggleGUI::labelCube(i, j, board.getLetter(i, j), false);
        }
    }
    cout << "It's your turn!" << endl;
    BoggleGUI::setStatusMessage("It's your turn!");
    humanTurn(board);
    cout << endl << "It's my turn!" << endl;
    BoggleGUI::setStatusMessage("It's my turn!");
    computerTurn(board);
    printResultOfGame(board);
}

/* This checks whether the Boggle GUI has already been set up when starting
 * a new game. This helps save some time and power if the board has already been
 * initialized.
 */
void initializeOrReset(int rowsAndCols) {
    if (BoggleGUI::isInitialized()) {
        BoggleGUI::reset();
        BoggleGUI::setStatusMessage("");
    } else {
        BoggleGUI::initialize(rowsAndCols, rowsAndCols);
        BoggleGUI::setAnimationDelay(100);
    }
}

/* This sets up the Boggle board by asking if the user wants to generate a random board.
 * Then, it sets up the board by calling the Boggle function. It returns this Boggle function, so
 * that the Boggle GUI can be updated.
 */
Boggle setUpBoard(Lexicon& dictionary, int rowsAndCols) {
    string userBoardText = "";
    if (!getYesOrNo("Do you want to generate a random board? ")) {
        while(!isValidUserBoard(userBoardText)) {
            cout << "That is not a valid 16-letter board string. Try again." << endl;
        }
    }
    return Boggle(dictionary, userBoardText, rowsAndCols);
}

/* If the user wants to generate their own board, this function asks the user
 * to input a 16-letter string. It checks if this user input is valid and contains
 * all alphabetic characters.
 */
bool isValidUserBoard(string& userBoardText) {
    userBoardText = getLine("Type the 16 letters to appear on the board: ");
    if (userBoardText.length() != 16) return false;
    userBoardText = toUpperCase(userBoardText);
    for (int i = 0; i < userBoardText.size(); i++) {
        if (!isalpha(userBoardText[i])) return false;
    }
    return true;
}

/* This plays the human's turn. It runs continually until the user enters
 * an empty string for a word. It prints the human status of what words have been
 * found and the human's score. It calls the human word search function to see
 * if the word specified by the user is on the board. A message is printed whether
 * or not the word has been found.
 */
void humanTurn(Boggle& board) {
    string humanWord;
    bool validWord;
    while (true) {
        cout << board << endl;
        printHumanStatus(board);
        humanWord = getLine("Type a word (or Enter to stop): ");
        if (humanWord.empty()) break;
        humanWord = toLowerCase(humanWord);
        validWord = board.humanWordSearch(humanWord);
        printHumanWordMessage(validWord, humanWord);
        clearConsole();
    }
}

/* This plays the computer's turn. It composes a set of all of the words
 * that are valid on the board and prints out those words in alphabetical order
 * and then the computer's score.
 */
void computerTurn(Boggle& board) {
    Set<string> computerWords = board.computerWordSearch();
    cout << "My words (" << computerWords.size() << "): " << computerWords << endl;
    cout << "My score: " << board.getComputerScore() << endl;
    for (string word:computerWords) {
        BoggleGUI::recordWord(word, BoggleGUI::COMPUTER);
    }
    BoggleGUI::setScore(board.getComputerScore(), BoggleGUI::COMPUTER);
}

/* This prints the result of the game. If the score difference is greater than 0,
 * the computer has won the game. If it is less than or equal to 0, the human has won.
 */
void printResultOfGame(Boggle& board) {
    string message;
    int scoreDiff = board.getComputerScore() - board.getHumanScore();
    if (scoreDiff > 0) {
        message = "Ha ha ha, I destroyed you. Better luck next time, puny human!";
    } else if (scoreDiff <= 0) {
        message = "Congratulations, you beat me! I'll get you next time!";
    }
    printMessageToConsoleAndGUI(message);
}

/* This prints the status of the human of the words that have been found and the human's score.
 */
void printHumanStatus(Boggle& board) {
    cout << "Your words (" << board.getNumHumanWords() << "): " << board.getHumanWords() << endl;
    cout << "Your score: " << board.getHumanScore() << endl;
    BoggleGUI::setScore(board.getHumanScore(), BoggleGUI::HUMAN);
}

/* This states whether the user has found a new word or the inputted word is
 * either already found or not valid.
 */
void printHumanWordMessage(bool validWord, string humanWord) {
    string message;
    if (validWord) {
        message = "You found a new word! \"" + humanWord + "\"";
        BoggleGUI::recordWord(humanWord, BoggleGUI::HUMAN);
    } else {
        message = "You must enter an unfound 4+ letter word from the dictionary.";
    }
    printMessageToConsoleAndGUI(message);
}

/* This sets the Boggle GUI status message to be whether the word input has been found
 * or not.
 */
void printMessageToConsoleAndGUI(string message) {
    cout << message << endl;
    BoggleGUI::setStatusMessage(message);
}
