/* Name: Kevin Manouchehri Section: Jean-Luc Watson, Friday 1:30pm
 * This encoding file contains functions that will perform the actions needed to encode and
 * decode files that include text, binary, image, or one that has already been compressed.
 * Once the compression or decompression is performed on the file, the contents are written to
 * the output stream.
 */

#include "encoding.h"
#include "pqueue.h"
#include "filelib.h"

/* This method builds the frequency table of characters from an input file or string.
 * It uses a map to store these frequencies and returns the resulting map at the end
 * of the file. It is assumed that the input file exists and can be read.
 */
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    int current = input.get();
    while (current != EOF) {
        freqTable[current]++;
        current = input.get();
    }
    freqTable.put(PSEUDO_EOF, 1);
    return freqTable;
}

/* This helper method creates the priority queue of each character and its frequency as
 * a Huffman node. It is then used later to make the encoding tree.
 */
void makePQ(PriorityQueue<HuffmanNode*>& pq, const Map<int, int>& freqTable) {
    Vector<int> keys = freqTable.keys();
    for (int key:keys) {
        HuffmanNode* node = new HuffmanNode(key, freqTable[key]);
        pq.enqueue(node, freqTable[key]);
    }
}

/* This is a helper function that makes the encoding tree out of the priority queue of
 * Huffman nodes that have been created by makePQ. The tree's structure is such that
 * the most frequent characters are near the top root and the least frequent characters
 * are leaves on the bottom.
 */
void makeTree(PriorityQueue<HuffmanNode*>& pq) {
    while (pq.size() > 1) {
        HuffmanNode* left = pq.dequeue();
        HuffmanNode* right = NULL;
        if (!pq.isEmpty()) right = pq.dequeue();
        HuffmanNode* parent = new HuffmanNode(NOT_A_CHAR, right->count + left->count, left, right);
        pq.enqueue(parent, right->count + left->count);
    }
}

/* This method accepts a frequency table and uses it to create a Huffman encoding tree based
 * on the frequencies. The frequencies are ordered from most to least frequent. The function
 * returns a pointer to the root of the tree.
 */
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode*> pq;
    makePQ(pq, freqTable);
    makeTree(pq);
    return pq.peek();
}

/* This is a helper function to the building of the encoding map. It recursivesly traverses through
 * the encoding tree and creates a binary encoding string to be stored in the map when a leaf is reached.
 */
void traverseEncode(HuffmanNode* encodingTree, Map<int, string>& encodingMap, string current) {
    if (encodingTree->isLeaf()) {
        encodingMap.put(encodingTree->character, current);
        return;
    }
    traverseEncode(encodingTree->zero, encodingMap, current + "0"); //left
    traverseEncode(encodingTree->one, encodingMap, current + "1");  //right
}

/* This function accepts a pointer to root node of the Huffman tree and uses it to create
 * a Huffman encoding map. The resulting encoding map is returned at the end and returned as
 * empty if the first node is NULL.
 */
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    if (encodingTree != NULL) traverseEncode(encodingTree, encodingMap, "");
    return encodingMap;
}

/* This is a helper function to the encodeData method to write the given bits to the output stream.
 * A string code of binary is passed into the function to be written out.
 */
void bitWriteHelper(obitstream& output, string current) {
    for (int i = 0; i < current.length(); i++) {
        if (current[i] == '0') {
            output.writeBit(0);
        } else if (current[i] == '1') {
            output.writeBit(1);
        }
    }
}

/* This method reads one character at a time from the input file and uses the encoding map
 * passed in by reference to encode each character to binary. These binary bits are then written
 * to the given output bit stream.
 */
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    rewindStream(input);
    int current = input.get();
    while (current != EOF) {
        bitWriteHelper(output, encodingMap.get(current));
        current = input.get();
    }
    bitWriteHelper(output, encodingMap.get(PSEUDO_EOF));
}

/* This helper function for the decoding method recursively goes through the encoded data depending on if
 * the current bit refers to the left or right branch. When it reaches a leaf, it puts that character on the
 * output stream.
 */
bool traverseDecode(ibitstream& input, HuffmanNode* encodingTree, ostream& output, HuffmanNode* current) {
    if (current->isLeaf()) {
        if (current->character == PSEUDO_EOF) {
            freeTree(encodingTree);
            return false;
        } else {
            output.put(current->character);
            return true;
        }
    }
    int bit = input.readBit();
    if (bit == 0) {
        return traverseDecode(input, encodingTree, output, current->zero);
    } else if (bit == 1) {
        return traverseDecode(input, encodingTree, output, current->one);
    }
    return true;
}

/* This method decodes the input by calling on the traverseDecode helper, which reads the bits from the input
 * and walks through the decoding tree to write the original uncompressed contents of the file to the output
 * stream.
 */
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    bool decode = true;
    while (decode) {
        decode = traverseDecode(input, encodingTree, output, encodingTree);
    }
}

/* This method compresses a file by creating an encoding tree utilizing a frequency
 * table and encoding the file with the tree. The content of the resulting encoded data
 * is printed to the output stream.
 */
void compress(istream& input, obitstream& output) {
    Map<int, int> freqTable = buildFrequencyTable(input);
    output << freqTable;
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(encodingTree);
    encodeData(input, encodingMap, output);
    freeTree(encodingTree);
}

/* This method decompresses a Huffman-encoded file (.huf file). It reads the input bits
 * from the input file one at a time, builds an encoding tree from the resulting frequency
 * table, and then decodes the file to write the contents to the output stream. It is
 * assumed that the streams are valid and read/writeable.
 */
void decompress(ibitstream& input, ostream& output) {
    Map<int, int> freqTable;
    input >> freqTable;
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    decodeData(input, encodingTree, output);
    freeTree(encodingTree);
}

/* This method frees the tree memory whose root node is represent by the pointer passed in.
 */
void freeTree(HuffmanNode* node) {
    if (node == NULL) { // empty tree
        return;
    } else if (node->isLeaf()) {
        delete node;
        return;
    }
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}
