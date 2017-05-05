/* Name: Kevin Manouchehri Section: Jean-Luc Watson, Friday 1:30pm
 * This programs reads in a user-specified grammar file and generates
 * three random sentences based on the contents of the file. Lecture slides
 * and Piazza were used as aid in developing this program.
 */

#include <iostream>
#include <fstream>
#include <string>
#include "console.h"
#include "simpio.h"   // for getLine
#include "strlib.h"   // for toLowerCase, trim
#include "map.h"
#include "random.h"

using namespace std;

static const string kGrammarsDirectory = "grammars/";
static const string kGrammarFileExtension = ".g";
static void makeDefinitions(ifstream &infile, Map<string, Vector<string>> &definition);
static void makeSentences(Map<string, Vector<string>> &definition);
static void eliminateNonterminals(Map<string, Vector<string>> &definition, string modWorking);

/* This returns a string that is the converted grammar file such that it can be read in.
 */
static string getNormalizedFilename(string filename) {
    string normalizedFileName = kGrammarsDirectory + filename;
    if (!endsWith(normalizedFileName, kGrammarFileExtension))
        normalizedFileName += kGrammarFileExtension;
    return normalizedFileName;
}

/* This is called upon by getFileName and determines whether the file name that has been supplied is
 * contained within the working directory.
 */
static bool isValidGrammarFilename(string filename) {
    string normalizedFileName = getNormalizedFilename(filename);
    ifstream infile(normalizedFileName.c_str());
    return !infile.fail();
}

/* This prompts the user for a grammar file name to be inputted or to quit the program. It returns the
 * file name if it is valid.
 */
static string getFileName() {
    while (true) {
        string filename = trim(getLine("Name of grammar file? [<return> to quit]: "));
        if (filename.empty() || isValidGrammarFilename(filename)) return filename;
        cout << "Failed to open the grammar file named \"" << filename << "\". Please try again...." << endl;
    }
}

/* This main creates a map to store the definitions that are included in the file input by the user. It calls
 * the method to add the definitions to the map and the method to create the three random sentences.
 */
int main() {
    cout << "Welcome to CS 106B Random Sentence Generator."
         << endl << "This program creates three random sentences based on a grammar file supplied."
         << endl << "Give me a grammar file, and I will create three random sentences for you." << endl
         << endl;
    while (true) {
        string filename = getFileName();
        string normalizedFileName = getNormalizedFilename(filename);
        if (filename.empty()) break;
        ifstream infile(normalizedFileName.c_str());
        Map<string, Vector<string>> definition;
        cout << endl;
        makeDefinitions(infile, definition);
        makeSentences(definition);
    }
    cout << "Thanks for playing!" << endl;
    return 0;
}

/* This method looks at the grammar file line-by-line and stores the definitions in a map
 * format. The map is sorted according to the nonterminal the expansion belongs to.
 */
static void makeDefinitions(ifstream &infile, Map<string, Vector<string>> &definition) {
    string line, expansion, nonterminal;
    while (getline(infile,line)) {
        if (startsWith(line, "<")) {
            int end = stringIndexOf(line, ">");
            nonterminal = line.substr(1,end-1);
            getline(infile,line); // this next line contains the number of expansions
            int numExpansions = stringToInteger(line);
            while (numExpansions > 0) {
                getline(infile,expansion);
                definition[nonterminal].add(expansion);
                numExpansions--;
            }
        }
    }
}

/* This method creates and prints the three random sentences based on the definition map.
 * This method calls the subsequent method that replaces each nonterminal it comes across
 * with an expansion.
 */
static void makeSentences(Map<string, Vector<string>> &definition) {
    int sentence = 1;
    while (sentence < 4) {
        string working = definition.get("start").toString();
        string modWorking = working.substr(2,working.size()-4); // this eliminates the {" and "}
        eliminateNonterminals(definition, modWorking);          // at the beginning and end
        cout << modWorking << endl << endl;
        sentence++;
    }
}

/* This method replaces each nonterminal encountered in the working string with one of its
 * expansions. After replacing the nonterminal, it checks if the working string contains another nonterminal,
 * and if it does, it recursively calls the method.
 */
static void eliminateNonterminals(Map<string, Vector<string>> &definition, string modWorking) {
    int start = stringIndexOf(modWorking, "<");
    int end = stringIndexOf(modWorking,">");
    string nonterminal = modWorking.substr(start+1,end-start-1);
    int numExpansions = definition[nonterminal].size();
    string temp;
    temp = definition[nonterminal][randomInteger(0,numExpansions-1)];
    modWorking = modWorking.substr(0,start) + temp + modWorking.substr(end+1,modWorking.size()-end-1);
    if (stringContains(modWorking,"<")) {
        eliminateNonterminals(definition, modWorking);
    }
}
