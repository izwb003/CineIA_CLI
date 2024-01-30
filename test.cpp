//
// Useless now. Keep for reference.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "cineia.h"

using namespace SMPTE::ImmersiveAudioBitstream;
using namespace std;

/*
int main() {
    ifstream* imfInput = new ifstream("2.iab", ios::binary);
    ofstream* atmosOutput = new ofstream("2.atmos", ios::binary);
    // Make output file buffer
    vector<char> atmosBuffer;
    uint32_t atmosFileSize = 0;
    iabError error;
    // Make conversion
    error = reassembleIAB(imfInput, atmosBuffer, atmosFileSize);
    // Copy PreambleValue
    copyPreambleValue(imfInput, atmosBuffer, atmosFileSize);
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
 */

std::string IntToStringWithLeadingZeros(int value, int width) {
    std::stringstream ss;
    ss << std::setw(width) << std::setfill('0') << value;
    return ss.str();
}

int main() {
    for(int frameNum = 0; ; frameNum ++) {
        string imfFileName = "IMFIAB/" + to_string(frameNum) + ".iab";
        ifstream* imfInput = new ifstream(imfFileName, ios::binary);
        if(!imfInput->is_open()) return 0;
        string atmosFileName = "DCPIAB/" + IntToStringWithLeadingZeros(frameNum, 6) + ".atmos";
        cout<<atmosFileName<<endl;
        ofstream* atmosOutput = new ofstream(atmosFileName, ios::out | ios::binary);
        // Make output file buffer
        vector<char> atmosBuffer;
        uint32_t atmosFileSize = 0;
        iabError error;
        // Make conversion
        error = reassembleIAB(imfInput, atmosBuffer, atmosFileSize);
        // Copy PreambleValue
        copyPreambleValue(imfInput, atmosBuffer, atmosFileSize);
        // Write output
        atmosOutput->write(atmosBuffer.data(), atmosFileSize);
        imfInput->close();
        atmosOutput->close();
        delete imfInput;
        imfInput = nullptr;
        delete atmosOutput;
        atmosOutput = nullptr;
    }
}