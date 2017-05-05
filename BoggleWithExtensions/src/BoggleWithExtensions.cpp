/* Name: Kevin Manouchehri Section: Friday 1:30pm, Jean-Luc Watson
 * This is the Boggle class program that implements the functions that keep track
 * of the progress of the human in finding words and then the computer finding all
 * possible words. It creates the Boggle board and uses the board to search for the
 * words.
 * EXTRA FEATURES: The Boggle board is now 5x5, and a sound is played after certain
 * scenarios. These scenarios include the user's word being successful, the word being
 * unsuccessful, the board being set up with rattling dice, and two different sounds being
 * played depending on the final score of the game.
 */

#include "BoggleWithExtensions.h"
#include <cmath>
#include "bogglegui.h"
#include "strlib.h"
#include "grid.h"
#include "shuffle.h"

// letters on all 6 sides of every cube
static string CUBES[16] = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

// letters on every cube in 5x5 "Big Boggle" version (extension)
static string BIG_BOGGLE_CUBES[25] = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

Boggle::Boggle(Lexicon& dictionary, string boardText, int rowsAndCols) {
    numRowsAndCols = rowsAndCols;
    boardGrid.resize(numRowsAndCols, numRowsAndCols);
    highlightedLetters.resize(numRowsAndCols, numRowsAndCols);
    if (boardText.empty()) { // user has selected a random board
        BoggleGUI::playSound("dice-rattle.wav");
        shuffle(BIG_BOGGLE_CUBES, numRowsAndCols^2);
        for (int i = 0; i < numRowsAndCols; i++) {
            for (int j = 0; j < numRowsAndCols; j++) {
                boardGrid.set(i, j, BIG_BOGGLE_CUBES[numRowsAndCols*i+j][randomInteger(0,5)]);
            }
        }
    } else {
        for (int i = 0; i < numRowsAndCols; i++) {
            for (int j = 0; j < numRowsAndCols; j++) {
                boardGrid.set(i, j, boardText[numRowsAndCols*i+j]);
            }
        }
    }
    dict = dictionary;
    humanScore = 0;
    computerScore = 0;
}

int Boggle::getNumRowsAndCols() {
    return numRowsAndCols;
}

char Boggle::getLetter(int row, int col) {
    try {
        if (row >= numRowsAndCols || row < 0 || col < 0 || col >= numRowsAndCols) throw -1;
    } catch (int e) {
        return ' ';
    }
    return boardGrid[row][col];
}

bool Boggle::checkWord(string word) {
    return (dict.contains(word) && word.length() >= 4 && !humanWords.contains(word));
}

bool Boggle::humanWordSearch(string word) {
    if (!checkWord(word)) return false;
    BoggleGUI::clearHighlighting();
    word = toUpperCase(word);
    highlightedLetters.fill(0);
    if (!findHumanWord(word, -1, -1)) return false;
    highlightedLetters.fill(0);
    humanScore += word.length() - 3;
    humanWords.add(word);
    return true;
}

string Boggle::getHumanWords() {
    return humanWords.toString();
}

int Boggle::getNumHumanWords() {
    return humanWords.size();
}

int Boggle::getHumanScore() {
    return humanScore;
}

int Boggle::getComputerScore() {
    return computerScore - humanScore;
}

bool Boggle::findHumanWord(string word, int row, int col) {
    if (word.length() == 0)
        return true; // base case
    Grid<int> neighbors = availableSpaces(row, col);
    for (int i = 0; i < numRowsAndCols; i++) {
        for (int j = 0; j < numRowsAndCols; j++) {
            if (neighbors.get(i,j) == 1) {
                BoggleGUI::setHighlighted(i, j, true);
                BoggleGUI::setAnimationDelay(100);
                if (getLetter(i,j) == word[0] && highlightedLetters.get(i, j) != 1) {
                    highlightedLetters.set(i, j, 1);
                    if (findHumanWord(word.substr(1, string::npos), i, j)) return true;
                    highlightedLetters.set(i, j, 0);
                    BoggleGUI::setHighlighted(i, j, false);
                } else if (highlightedLetters.get(i,j) != 1) {
                    BoggleGUI::setHighlighted(i, j, false);
                }
            }
        }
    }
    return false;
}

bool Boggle::findWordsFromBoardIndex(Set<string>& result, string prefix, int row, int col) {
    Grid <int> neighbors = availableSpaces(row, col);
    for (int i = 0; i < numRowsAndCols; i++) {
        for (int j = 0; j < numRowsAndCols; j++) {
            if (neighbors.get(i, j) == 1 && highlightedLetters.get(i, j) != 1) {
                prefix += boardGrid[i][j];
                prefix = toUpperCase(prefix);
                highlightedLetters.set(i, j, 1);
                if (checkWord(prefix) && !result.contains(prefix)) {
                    result.add(prefix);
                    computerScore += prefix.length() - 3;
                }
                if (dict.containsPrefix(prefix)) {
                    findWordsFromBoardIndex(result, prefix, i, j);
                }
                prefix = prefix.substr(0, prefix.length()-1);
                highlightedLetters.set(i, j, 0);
            }
        }
    }
    return false;
}

Grid<int> Boggle::availableSpaces(int row, int col) {
    Grid<int> availableSpaces;
    availableSpaces.resize(numRowsAndCols, numRowsAndCols);
    if (row == -1 && col == -1) {
        availableSpaces.fill(1);
    } else {
        for (int i = max(0, row - 1); i <= min(numRowsAndCols - 1, row + 1); i++) {
            for (int j = max(0, col - 1); j <= min(numRowsAndCols - 1, col + 1); j++) {
                if (i != row || j != col) {
                    availableSpaces.set(i, j, 1);
                }
            }
        }
    }
    return availableSpaces;
}

Set<string> Boggle::computerWordSearch() {
    Set<string> result;
    string prefix = "";
    highlightedLetters.fill(0);
    findWordsFromBoardIndex(result, prefix, -1, -1);
    return result;
}

void Boggle::findComputerWords(Set<string>& result) {
    string prefix = "";
    highlightedLetters.fill(0);
    findWordsFromBoardIndex(result, prefix, -1, -1);
}

ostream& operator<<(ostream& out, Boggle& boggle) {
    int numRows = boggle.getNumRowsAndCols();
    int numCols = numRows;
    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            out << boggle.getLetter(row, col);
        }
        out << endl;
    }
    return out;
}
