//
// Created by songd on 2024/1/7.
//

#include <fstream>
#include <iostream>

#include "cineia.h"

using namespace SMPTE::ImmersiveAudioBitstream;
using namespace std;

int main() {
    ifstream* imfInput = new ifstream("2.iab", ios::binary);
    ofstream* atmosOutput = new ofstream("2.atmos", ios::binary);
    // Make output file buffer
    vector<char> atmosBuffer;
    uint32_t atmosFileSize = 0;
    iabError error;
    // Make conversion
    error = reassembleIAB(imfInput, atmosBuffer, atmosFileSize);
    // Write output
    atmosOutput->write(atmosBuffer.data(), atmosFileSize);
    // Clean memory
    imfInput->close();
    atmosOutput->close();
    delete imfInput;
    imfInput = nullptr;
    delete atmosOutput;
    atmosOutput = nullptr;
    return 0;
}