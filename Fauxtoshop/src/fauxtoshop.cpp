/*
 * Name: Kevin Manouchehri Section: Jean-Luc Watson, Friday 1:30 PM
 * This program performs image modifications, including scattering, black and
 * white edge conversion, and green screen, depending on what the user desires.
 * Lecture slides and questions asked by students on Piazza were used as help
 * in developing this program. To help with the extensions, the Grayscale
 * Wikipedia and
 * http://www.techrepublic.com/blog/how-do-i/how-do-i-convert-images-to-grayscale-and-sepia-tone-using-c/
 * were utilized.
 */

#include <iostream>
#include "console.h"
#include "gwindow.h"
#include "grid.h"
#include "simpio.h"
#include "strlib.h"
#include "gbufferedimage.h"
#include "gevents.h"
#include "math.h" //for sqrt and exp in the optional Gaussian kernel
#include "random.h"
using namespace std;

static const int    WHITE = 0xFFFFFF;
static const int    BLACK = 0x000000;
static const int    GREEN = 0x00FF00;
static const double PI    = 3.14159265;

static void     doFauxtoshop(GWindow &gw, GBufferedImage &img);
static void     promptForFile(GWindow &gw, GBufferedImage &img, int& quit);
static void     mainMenu(GBufferedImage &img);
static void     promptForSave(GBufferedImage &img);
static void     scatter(GBufferedImage &img);
static void     edgeDetection(GBufferedImage &img);
static void     greenScreen(GBufferedImage &img);
static void     compareImages(GBufferedImage &img);
static void     colorFilter(GBufferedImage &img);
static void     grayscale(GBufferedImage &img);
static void     sepia(GBufferedImage &img);
static void     negative(GBufferedImage &img);

static bool     openImageFromFilename(GBufferedImage& img, string filename);
static bool 	saveImageToFilename(const GBufferedImage &img, string filename);
static void     getMouseClickLocation(int &row, int &col);

/* STARTER CODE FUNCTION - DO NOT EDIT
 *
 * This main simply declares a GWindow and a GBufferedImage for use
 * throughout the program. By asking you not to edit this function,
 * we are enforcing that the GWindow have a lifespan that spans the
 * entire duration of execution (trying to have more than one GWindow,
 * and/or GWindow(s) that go in and out of scope, can cause program
 * crashes).
 */
int main() {
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setVisible(true);
    GBufferedImage img;
    doFauxtoshop(gw, img);
    return 0;
}

/* This contains the meat of the Fauxtoshop program. It starts with the welcome message.
 * Then, it prompts the user for an image file to open and make modifications to. If
 * the user has not specified that they want to quit (blank) and has successfully loaded
 * an image, then they are asked what they would like to do with the image. They have four
 * options that involve scattering the image, making the image black and white, placing
 * another sticker image on top of the original image, and comparing another image to the
 * original. After the user has worked with the modifications they desire, they are asked
 * if they would like to save their modification. They can either save or skip the step.
 * After that, the window is cleared, and the Fauxtoshop program begins again.
 */
static void doFauxtoshop(GWindow &gw, GBufferedImage &img) {
    int quit = 0;
    while (quit == 0) {
        cout << "Welcome to Fauxtoshop!" << endl;
        promptForFile(gw, img, quit);
        if (quit == 0) { // if the user did not enter blank when prompted for file to load
            mainMenu(img); // main menu options
            promptForSave(img); // time to save
            gw.clear(); // clear the window
            cout << endl;
        } else { // if the user has entered blank to quit
            cout << "Thanks for using Fauxtoshop!" << endl;
        }
    }
}

/* This prompts the user for an image file to load. If the name given is valid name,
 * it loads the file and resizes the window to fit the image. If the name given is
 * not valid, the user is reprompted to give an image file. The quit integer is
 * passed by reference in, and if the user enters a blank file name, it is taken
 * to mean that user wants to quit.
 */
static void promptForFile(GWindow &gw, GBufferedImage &img, int& quit) {
    while (true) {
        string imageFile = getLine("Enter name of image file to open (or blank to quit): ");
        if (imageFile.length() >= 1) {
            cout << "Opening image file; may take a minute..." << endl;
            if (openImageFromFilename(img, imageFile)) {    // if true, the image is opened
                openImageFromFilename(img, imageFile);      // and loaded onto the window
                gw.setCanvasSize(img.getWidth(), img.getHeight());
                gw.add(&img,0,0);
                break;
            } else {
                cout << "Sorry, that is not a valid filename. Please try again." << endl;
            }
        } else {      // if the user entered a blank file name
            quit++;
            break;
        }
    }
}

/* This sets the main menu of the Fauxtoshop program. It gives the user four options
 * of things to do with the image they have loaded. If the user inputs something
 * other than the numerals 1-4, then they are reprompted. With each choice entered,
 * different modifications are made to the image, which are scatter, edge detection,
 * green screen, and comparing two images.
 */
static void mainMenu(GBufferedImage &img) {
    while (true) {
        cout << "Which image filter would you like to apply?"
             << endl << "   1 - Scatter"
             << endl << "   2 - Edge detection"
             << endl << "   3 - 'Green screen' with another image"
             << endl << "   4 - Compare image with another image"
             << endl << "   5 - Color Effects Filters"
             << endl;
        int choice = getInteger("Your choice: ");
        if (choice == 1) {
            scatter(img);
            break;
        } else if (choice == 2) {
            edgeDetection(img);
            break;
        } else if (choice == 3) {
            greenScreen(img);
            break;
        } else if (choice == 4) {
            compareImages(img);
            break;
        } else if (choice == 5) {
            colorFilter(img);
            break;
        } else {
            cout << "Sorry, that is not a valid choice. Please try again." << endl;
        }
    }
}

/* This runs after the user has made their desired modifications to the image.
 * It prompts for a file name to save the modified image to. If the user enters
 * in a blank file name, it is assumed that they do not want to save the modified
 * image. If an invalid file name is entered, the user is reprompted.
 */
static void promptForSave(GBufferedImage &img) {
    while (true) {
        string saveFile = getLine("Enter filename to save image (or blank to skip saving): ");
        if (saveFile.length() >= 1) {
            if (saveImageToFilename(img, saveFile)) {
                saveImageToFilename(img, saveFile);
                break;
            } else {
                cout << "Sorry, that is not a valid filename. Please try again." << endl;
            }
        } else if (saveFile.length() == 0) {  // assumed to mean the user wants to
            break;                            // skip saving
        }
    }
}

/* Scatter takes the original image that the user has loaded and "scatters" the
 * pixels. It asks the user to input how far to scatter the pixels. Within the range
 * of this degree of scatter from each pixel, a row and column are randomly selected,
 * and the pixel at that randomly selected point provides the color for the pixel.
 *
 */
static void scatter(GBufferedImage &img) {
    int radius = 0;
    while (radius < 1 || radius > 100) {
        radius = getInteger("Enter degree of scatter [1-100]: ");
    }
    Grid<int> original = img.toGrid(); // convert the image to a grid that can
    int rows = original.numRows();     // be modified
    int columns = original.numCols();
    Grid<int> copy(rows, columns);
    int pixRow, pixCol;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            pixRow = randomInteger(i - radius, i + radius);
            while (!original.inBounds(pixRow, j)) { // if the selected pixel is out
                pixRow = randomInteger(i - radius, i + radius); // of bounds of the
            }                                                   // grid
            pixCol = randomInteger(j - radius, j + radius);
            while (!original.inBounds(i, pixCol)) { // if the selected pixel is out
                pixCol = randomInteger(j - radius, j + radius); // of bounds of the
            }                                                   // grid
            int pixel = original[pixRow][pixCol]; // get the color of selected pixel
            copy.set(i,j, pixel); // set current pixel to be color of randomly
        }                         // selected pixel
    }
    img.fromGrid(copy); // grid converted back to image
}

/* Edge detection turns the original image the user loaded into black and white.
 * A pixel is converted to black if it was an edge in the original image and to
 * white if it was not an edge. As an input, the method asks for the user to
 * supply an edge threshold that provides the marker for how different two adjacent
 * pixels must be to be considered an "edge." If their difference exceeds this
 * threshold, then the main pixel is declared an "edge." For each pixel, the method
 * calculates the difference between the pixel and each of its 9 neighbors
 * (immediately adjacent, diagonal, and itself). The method utilizes the inBounds
 * method in the Grid class to handle pixels that are near edges and corners. The
 * definition of difference between pixels is the maximum difference between amount
 * of one color (red, green, or blue) for two pixels.
 */
static void edgeDetection(GBufferedImage &img) {
    int threshold = 0;
    while (threshold < 1) { // threshold must be a positive integer
        threshold = getInteger("Enter threshold for edge detection: ");
    }
    Grid<int> original = img.toGrid();
    int rows = original.numRows();
    int columns = original.numCols();
    Grid<int> copy(rows, columns);
    int pixelSelf, redSelf, greenSelf, blueSelf; // the colors of the current pixel
    int pixel2, red2, green2, blue2; // the colors of one of the neighbors
    int redDiff, greenDiff, blueDiff;
    int pixelDiff, edge;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            edge = 0;
            pixelSelf = original[i][j];
            // the following obtains the amount of red, green, and blue in the pixel
            GBufferedImage::getRedGreenBlue(pixelSelf, redSelf, greenSelf, blueSelf);
            for (int rowNeighbor = -1; rowNeighbor < 2; rowNeighbor++) {
                for (int colNeighbor = -1; colNeighbor < 2; colNeighbor++) {
                    if (original.inBounds(i + rowNeighbor, j + colNeighbor)) {
                        pixel2 = original[i + rowNeighbor][j + colNeighbor];
                        GBufferedImage::getRedGreenBlue(pixel2, red2, green2, blue2);
                        redDiff = abs(redSelf - red2);
                        greenDiff = abs(greenSelf - green2);
                        blueDiff = abs(blueSelf - blue2);
                        // finds which color has the max diff
                        if (redDiff >= greenDiff) {
                            if (redDiff >= blueDiff) {
                                pixelDiff = redDiff;
                            } else {
                                pixelDiff = blueDiff;
                            }
                        } else {
                            if (greenDiff >= blueDiff) {
                                pixelDiff = greenDiff;
                            } else {
                                pixelDiff = blueDiff;
                            }
                        }
                        if (pixelDiff > threshold) { // if true, we have an edge
                            edge++;
                        }
                    }
                }
            }
            if (edge >= 1) {
                copy.set(i,j,BLACK);
            } else {
                copy.set(i,j,WHITE);
            }
        }
    }
    img.fromGrid(copy);
}

/* This method places a "sticker" image on top of the original image the user
 * loaded and ignores any part of the sticker image that is close to pure green.
 * The method begins by asking for the sticker image the user wants to use.
 * After successfully opening the file (and reprompting if not), the user, similar
 * to the edge detection method, is asked for the tolerance for the difference
 * marker from pure green we will use for each pixel. If the amount of green that
 * a given pixel contains differs by more than the threshold from pure green, that
 * pixel from the sticker is copied onto the background image. However, if it does
 * not exceed the threshold, the background pixel at that point is untouched. In
 * addition, the method obtains from the user where to place the sticker image by
 * either a mouse click specification or a manual input.
 */
static void greenScreen(GBufferedImage &img) {
    cout << "Now choose another file to add to your background image." << endl;
    GBufferedImage stickerImage;
    Grid<int> original = img.toGrid();
    while (true) { // loads the sticker image into a separated GBufferedImage
        string sticker = getLine("Enter name of image file to open: ");
        if (sticker.length() >= 1) {
            cout << "Opening image file; may take a minute..." << endl;
            if (openImageFromFilename(stickerImage, sticker)) {
                openImageFromFilename(stickerImage, sticker);
                break;
            }
        }
    }
    Grid<int> stickerGrid = stickerImage.toGrid();
    int tolThresh = 0;
    while (tolThresh < 1) { // want a positive threshold that is an integer
        tolThresh = getInteger("Now choose a tolerance threshold: ");
    }
    string location = getLine("Enter location to place image as '(row,col)' (or blank to use mouse): ");
    int placeRow, placeCol;
    if (location.length() > 0) { // if the user has specified a location manually
        string rowString = location.substr(1,location.find(",")-1);
        placeRow = stringToInteger(rowString);
        string colString = location.substr(location.find(",")+1, location.length() - location.find(",") - 2);
        placeCol = stringToInteger(colString);
    } else { // if the user wants to use a mouse click to choose a location
        cout << "Now click the background image to place new image." << endl;
        getMouseClickLocation(placeRow, placeCol); // obtains the location of mouse click
        cout << "You chose (" << placeRow << "," << placeCol << ")" << endl;
    }
    int stickerRows = stickerGrid.numRows();
    int stickerColumns = stickerGrid.numCols();
    int pixelPure = GREEN; // int of pure green
    int redPure, greenPure, bluePure;
    int pixelSticker, redSticker, greenSticker, blueSticker;
    int redDiff, greenDiff, blueDiff;
    int pixelDiff;
    // amount of each color in pure green
    GBufferedImage::getRedGreenBlue(pixelPure, redPure, greenPure, bluePure);
    for (int i = 0; i < stickerRows; i++) {
        for (int j = 0; j < stickerColumns; j++) {
            pixelSticker = stickerGrid[i][j];
            // amount of color in sticker image
            GBufferedImage::getRedGreenBlue(pixelSticker, redSticker, greenSticker, blueSticker);
            redDiff = abs(redSticker - redPure);
            greenDiff = abs(greenSticker - greenPure);
            blueDiff = abs(blueSticker - bluePure);
            if (redDiff >= greenDiff) {
                if (redDiff >= blueDiff) {
                    pixelDiff = redDiff;
                } else {
                    pixelDiff = blueDiff;
                }
            } else {
                if (greenDiff >= blueDiff) {
                    pixelDiff = greenDiff; // this is the guiding pixelDiff
                } else {
                    pixelDiff = blueDiff;
                }
            }
            if (pixelDiff > tolThresh && original.inBounds(i+placeRow,j+placeCol)) {
                original.set(i+placeRow,j+placeCol,pixelSticker);
            } // if we exceed the threshold, we place the sticker pixel on the background
        }
    }
    img.fromGrid(original);
}

/* This method utilizes the image that the user has already loaded and has the user
 * load another image to compare to it. It then counts the number of different pixels
 * between the two and then provides an output of that amount. If the same pictures
 * are loaded are compared, the output reads that the images are the same.
 */
static void compareImages(GBufferedImage &img) {
    cout << "Now choose another image file to compare to." << endl;
    GBufferedImage otherImg;
    while (true) { // load other image
        string compare = getLine("Enter name of image file to open: ");
        if (compare.length() >= 1) {
            cout << "Opening image file; may take a minute..." << endl;
            if (openImageFromFilename(otherImg, compare)) {
                openImageFromFilename(otherImg, compare);
                break;
            }
        }
    }
    int pixelCount = img.countDiffPixels(otherImg); // uses the GBufferedImage
    if (pixelCount > 0) {                           // countDiffPixels method
        cout << "These images differ in " << pixelCount << " pixel locations!" << endl;
    } else { // if not pixels are different
        cout << "These images are the same!" << endl;
    }
}

/* EXTENSION: This method allows the user to apply three different filters to the image
 * they have loaded. These three filters are grayscale, sepia tone, and negative. When
 * the user enters the integer corresponding to the specific filter, that filter's
 * method is called.
 */
static void colorFilter(GBufferedImage &img) {
    cout << "Now choose which color effects filter you would like to apply" << endl;
    while (true) {
        cout << "   1 - Grayscale"
             << endl << "   2 - Sepia tone"
             << endl << "   3 - Negative"
             << endl;
        int filter = getInteger("Your choice: ");
        if (filter == 1) {
            grayscale(img);
            break;
        } else if (filter == 2) {
            sepia(img);
            break;
        } else if (filter == 3) {
            negative(img);
            break;
        } else {
            cout << "Sorry that is not a valid choice. Please try again." << endl;
        }
    }
}

/* EXTENSION: This method changes the image into grayscale. The formula obtained to
 * convert the RGB integers into grayscale was obtained from the Grayscale Wikipedia
 * page. This calculated value is then used as the red, green, and blue integers to
 * turn each pixel into a shade of gray to be used on the image.
 */
static void grayscale(GBufferedImage &img) {
    Grid<int> original = img.toGrid();
    int rows = original.numRows();
    int columns = original.numCols();
    int pixel, red, green, blue; // the colors of the current pixel
    int grayscale, newPix;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            pixel = original[i][j];
            // the following obtains the amount of red, green, and blue in the pixel
            GBufferedImage::getRedGreenBlue(pixel, red, green, blue);
            grayscale = 0.299*red + 0.587*green + 0.114*blue; // convert to grayscale mode
            // sets the red, green, and blue values to be the same
            newPix = GBufferedImage::createRgbPixel(grayscale, grayscale, grayscale);
            original.set(i,j,newPix);
        }
    }
    img.fromGrid(original);
}

/* EXTENSION: This method puts a sepia tone filter onto the image. It obtains the red,
 * green, and blue values of the pixel and then modifies it according to equations
 * recommended by Microsoft on
 * http://www.techrepublic.com/blog/how-do-i/how-do-i-convert-images-to-grayscale-and-sepia-tone-using-c/.
 * If the modified color value exceeds 255, then the value is set to 255.
 */
static void sepia(GBufferedImage &img) {
    Grid<int> original = img.toGrid();
    int rows = original.numRows();
    int columns = original.numCols();
    int pixel, red, green, blue; // the colors of the current pixel
    int outputRed, outputGreen, outputBlue;
    int sep;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            pixel = original[i][j];
            // the following obtains the amount of red, green, and blue in the pixel
            GBufferedImage::getRedGreenBlue(pixel, red, green, blue);
            // sepia equations recommended by Microsoft
            outputRed = 0.393 * red + 0.769 * green + 0.189 * blue;
            outputGreen = 0.349 * red + 0.686 * green + 0.168 * blue;
            outputBlue = 0.272 * red + 0.534 * green + 0.131 * blue;
            if (outputRed > 255) {
                outputRed = 255;
            }
            if (outputGreen > 255) {
                outputGreen = 255;
            }
            if (outputBlue > 255) {
                outputBlue = 255;
            }
            // creates the pixel integer from the red, green, and blue values
            sep = GBufferedImage::createRgbPixel(outputRed, outputGreen, outputBlue);
            original.set(i,j,sep);
        }
    }
    img.fromGrid(original);
}

/* EXTENSION: This method turns the image into a negative. This is done by reversing
 * the red, green, and blue values and then turning them back into a pixel integer.
 */
static void negative(GBufferedImage &img) {
    Grid<int> original = img.toGrid();
    int rows = original.numRows();
    int columns = original.numCols();
    int pixel, red, green, blue; // the colors of the current pixel
    int neg;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            pixel = original[i][j];
            // the following obtains the amount of red, green, and blue in the pixel
            GBufferedImage::getRedGreenBlue(pixel, red, green, blue);
            // reverses the color values
            red = 255 - red;
            green = 255 - green;
            blue = 255 - blue;
            // creates the pixel integer from the red, green, and blue values
            neg = GBufferedImage::createRgbPixel(red, green, blue);
            original.set(i,j,neg);
        }
    }
    img.fromGrid(original);
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to open the image file 'filename'.
 *
 * This function returns true when the image file was successfully
 * opened and the 'img' object now contains that image, otherwise it
 * returns false.
 */
static bool openImageFromFilename(GBufferedImage& img, string filename) {
    try { img.load(filename); }
    catch (...) { return false; }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to save the image file to 'filename'.
 *
 * This function returns true when the image was successfully saved
 * to the file specified, otherwise it returns false.
 */
static bool saveImageToFilename(const GBufferedImage &img, string filename) {
    try { img.save(filename); }
    catch (...) { return false; }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Waits for a mouse click in the GWindow and reports click location.
 *
 * When this function returns, row and col are set to the row and
 * column where a mouse click was detected.
 */
static void getMouseClickLocation(int &row, int &col) {
    GMouseEvent me;
    do {
        me = getNextEvent(MOUSE_EVENT);
    } while (me.getEventType() != MOUSE_CLICKED);
    row = me.getY();
    col = me.getX();
}

/* OPTIONAL HELPER FUNCTION
 *
 * This is only here in in case you decide to impelment a Gaussian
 * blur as an OPTIONAL extension (see the suggested extensions part
 * of the spec handout).
 *
 * Takes a radius and computes a 1-dimensional Gaussian blur kernel
 * with that radius. The 1-dimensional kernel can be applied to a
 * 2-dimensional image in two separate passes: first pass goes over
 * each row and does the horizontal convolutions, second pass goes
 * over each column and does the vertical convolutions. This is more
 * efficient than creating a 2-dimensional kernel and applying it in
 * one convolution pass.
 *
 * This code is based on the C# code posted by Stack Overflow user
 * "Cecil has a name" at this link:
 * http://stackoverflow.com/questions/1696113/how-do-i-gaussian-blur-an-image-without-using-any-in-built-gaussian-functions
 *
 */
static Vector<double> gaussKernelForRadius(int radius) {
    if (radius < 1) {
        Vector<double> empty;
        return empty;
    }
    Vector<double> kernel(radius * 2 + 1);
    double magic1 = 1.0 / (2.0 * radius * radius);
    double magic2 = 1.0 / (sqrt(2.0 * PI) * radius);
    int r = -radius;
    double div = 0.0;
    for (int i = 0; i < kernel.size(); i++) {
        double x = r * r;
        kernel[i] = magic2 * exp(-x * magic1);
        r++;
        div += kernel[i];
    }
    for (int i = 0; i < kernel.size(); i++) {
        kernel[i] /= div;
    }
    return kernel;
}
