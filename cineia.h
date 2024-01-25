//
// Created by songd on 2024/1/7.
//

#ifndef CINEIA_CLI_CINEIA_H
#define CINEIA_CLI_CINEIA_H

#include <IABErrors.h>
#include <common/IABElements.h>

using namespace SMPTE::ImmersiveAudioBitstream;

void showError(iabError error);

iabError reassembleIAB(std::istream* iInputStream, std::vector<char> &oOutputBuffer, uint32_t &oOutputLength);

void copyPreambleValue(std::istream* iInputStream, std::ostream* oOutputStream);

#endif //CINEIA_CLI_CINEIA_H
