/* Name: Kevin Manouchehri Section: Friday 1:30pm, Jean-Luc Watson
 * This program creates a computer solution of the Marble Solitaire such that when
 * the user clicks the "Solve it for me!" button, the program finds how to get down
 * to one marble if possible. Lecture slides and Piazza were used as aid in developing
 * this program.
 */

#include <iostream>
#include "grid.h"
#include "set.h"
#include "vector.h"
#include "simpio.h"
#include "console.h"
#include "random.h"
#include "gevents.h"
#include "filelib.h"
#include "shuffle.h"

#include "marbletypes.h"
#include "marblegraphics.h"
#include "compression.h"

using namespace std;

bool checkForSolution(Grid<MarbleType>& board);
bool attemptToMakeMoves(Grid<MarbleType>& board, Set<uint32_t>& exploredBoards, Vector<Move>& moveHistory);

/* Provided helper functions (do not edit these, but you will call them) */
void makeMove(Move move, Grid<MarbleType>& board);
void undoMove(Move move, Grid<MarbleType>& board);
Vector<Move> findPossibleMoves(const Grid<MarbleType>& board);


/* This method returns true if it is able find a solution to the game and false
 * if it unable to. It makes moves one at a time and undoes the move if it does
 * not lead recursively to a solution.
 */
bool solvePuzzle(Grid<MarbleType>& board, Set<uint32_t>& exploredBoards,
                 Vector<Move>& moveHistory){
    if (checkForSolution(board)) return true;
    // use this to prune dupicate branches of search
    uint32_t encoding = compressMarbleBoard(board);
    if (exploredBoards.contains(encoding)) return false;
    exploredBoards.add(encoding);
    if (attemptToMakeMoves(board, exploredBoards, moveHistory)) return true;
    return false;
}

/* This helper function allows the solvePuzzle method to see if the last move made
 * has led to a solution. A solution is defined as only one marble remaining on the
 * board.
 */
bool checkForSolution(Grid<MarbleType>& board) {
    int marblesOnBoard = 0;
    for (int row=0; row < board.numRows(); row++){
        for (int col=0; col < board.numCols(); col++){
            if (board[row][col] == MARBLE_OCCUPIED) {
                marblesOnBoard += 1;
            }
        }
    }
    if (marblesOnBoard == 1) return true;
    return false;
}

bool attemptToMakeMoves(Grid<MarbleType>& board, Set<uint32_t>& exploredBoards, Vector<Move>& moveHistory) {
    Vector<Move> possibilities = findPossibleMoves(board);
    for (int i = 0; i < possibilities.size(); i++) {
        makeMove(possibilities[i], board);
        moveHistory.add(possibilities[i]);
        if (solvePuzzle(board, exploredBoards, moveHistory)) return true;
        moveHistory.remove(moveHistory.size() - 1);
        undoMove(possibilities[i], board);
    }
    return false;
}


/**********************************************************************************
 * YOU DO NOT NEED TO EDIT CODE BELOW THIS POINT                                  *
 * - however, you should use the functions below in your solvePuzzle function     *                                                        *                                                                   *
 * ********************************************************************************/

/* Performs the specified move on the board.
 * Precondition: this move must be valid.
 */
void makeMove(Move move, Grid<MarbleType>& board){
    int jumpedRow = move.startRow + (move.endRow - move.startRow) / 2;
    int jumpedCol = move.startCol + (move.endCol - move.startCol) / 2;
    board[move.startRow][move.startCol] = MARBLE_EMPTY;
    board[move.endRow][move.endCol] = MARBLE_OCCUPIED;
    board[jumpedRow][jumpedCol] = MARBLE_EMPTY;
}

/* Undoes the specified move on the board.
 * Note: it's important to realize that this method is
 * necessary because you have to replace the jumped marble
 * so you can't just call makeMove() with the start and end
 * locations swapped.
 */
void undoMove(Move move, Grid<MarbleType>& board){
    int jumpedRow = move.startRow + (move.endRow - move.startRow) / 2;
    int jumpedCol = move.startCol + (move.endCol-move.startCol) / 2;
    board[move.startRow][move.startCol] = MARBLE_OCCUPIED;
    board[move.endRow][move.endCol] = MARBLE_EMPTY;
    board[jumpedRow][jumpedCol] = MARBLE_OCCUPIED;
}

/* This examines the provided board and returns a Vector of
 * next moves that are possible for this board.
 */
Vector<Move> findPossibleMoves(const Grid<MarbleType>& board){
    Vector<Move> moves;
    for (int row=0; row<board.numRows(); row++){
        for (int col=0; col<board.numCols(); col++){
            if (board[row][col] == MARBLE_EMPTY) {
                if (row-2 >= 0 && board[row-2][col] == MARBLE_OCCUPIED
                        && board[row-1][col] == MARBLE_OCCUPIED){
                    moves.add(Move(row-2, col, row, col));
                }
                if (row+2 < board.numRows() && board[row+2][col] == MARBLE_OCCUPIED
                        && board[row+1][col] == MARBLE_OCCUPIED){
                    moves.add(Move(row+2, col, row, col));
                }
                if (col-2 >= 0 && board[row][col-2] == MARBLE_OCCUPIED
                        && board[row][col-1] == MARBLE_OCCUPIED){
                    moves.add(Move(row, col-2, row, col));
                }
                if (col+2 < board.numCols() && board[row][col+2] == MARBLE_OCCUPIED
                        && board[row][col+1] == MARBLE_OCCUPIED){
                    moves.add(Move(row, col+2, row, col));
                }
            }
        }
    }
    if (RANDOMIZED_POSSIBLE_MOVES) {
        shuffle(moves);
    }
    return moves;
}
