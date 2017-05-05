/* Name: Kevin Manouchehri Section: Friday 1:30pm, Jean-Luc Watson
 * This program runs the two method choices a user has when running the main recursion
 * program. The first method finds the amount of weight on each person's knees in
 * a human pyramid in which each row contains one fewer person than the row below
 * it. The second method draws a Siepinski triangle fractal image given a user input
 * of fractal order. Lecture slids and Piazza were used as aid in developing this
 * program.
 */

#include "recursionproblems.h"
#include <cmath>
#include <iostream>
#include "hashmap.h"
#include "map.h"
#include "random.h"
using namespace std;

void drawTriangle(GWindow& gw, double x, double y, int size);

/* This method takes the weight of each individual person and adds half of the respective weights
 * of the two people above them to the left and right (if applicable).
 */
double weightOnKnees(int row, int col, const Vector<Vector<double> >& weights) {
    if (row < 0 || col < 0 || row >= weights.size() || col >= weights[row].size()) return 0.0;
    if (row == 0 && col == 0) return weights[row][col]; // top of pyramid
    double aboveLeftPerson = weightOnKnees(row - 1, col - 1, weights);
    double aboveRightPerson = weightOnKnees(row - 1, col, weights);
    return weights[row][col] + 0.5 * aboveLeftPerson + 0.5 * aboveRightPerson;
}

/* This method draws a Sierpinski triangle according to the fractal order the user specifies.
 * It draws three Sierpinski triangles of order K-1 for a K user input.
 */
void drawSierpinskiTriangle(GWindow& gw, double x, double y, int size, int order) {
    if (x < 0 || y < 0 || size < 0 || order < 0) error("No negative values allowed.");
    if (order == 0) return;
    if (order == 1) {
        drawTriangle(gw, x, y, size);
        return;
    }
    drawSierpinskiTriangle(gw, x, y, size/2, order - 1);
    drawSierpinskiTriangle(gw, x + (size/2), y, size/2, order - 1);
    drawSierpinskiTriangle(gw, x + (size/4), y + (sqrt(3)/2) * (size/2), size/2, order - 1);
}

/* This helper function draws each triangle in the Sierpinski triangle method by drawing three
 * individual lines that connect.
 */
void drawTriangle(GWindow& gw, double x, double y, int size) {
   double height = y + sqrt(3)/2 * size; // equilateral triangle height formula
   gw.drawLine(x, y, x + size/2, height);
   gw.drawLine(x + size, y, x + size/2, height);
   gw.drawLine(x, y, x + size, y);

}
