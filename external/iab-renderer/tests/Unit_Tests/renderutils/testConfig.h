/*======================================================================*
    Copyright (c) 2015-2023 DTS, Inc. and its affiliates.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *======================================================================*/

//
//  testconfig.h
//  Unit Test
//

#ifndef TESTCONFIFG_H_
#define TESTCONFIFG_H_

/*######################################################
 Unit test configuration with 9.1 OH soundfield
######################################################*/

const std::string IABConfigWith91OHSoundfield=
    "v 3\n"
    "k   rpversion   2.2\n"
    "k   authtoolversion   dc67654de6\n"
    "k   authtool   tag:dts.com,2016:mda:authtool:olc\n"
    "c   smooth   1\n"
    "e   http://dts.com/mda/soundfield#91OH\n"
    "s   L        0   -30.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020101.00000000\n"
    "s   C        1     0.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020103.00000000\n"
    "s   R        2    30.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020102.00000000\n"
    "s   LSS      3   -90.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020107.00000000\n"
    "s   RSS      4    90.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020108.00000000\n"
    "s   LRS      5  -150.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020109.00000000\n"
    "s   RRS      6   150.00    0.00   urn:smpte:ul:060E2B34.0401010D.0302010A.00000000\n"
    "s   LFE      7     0.00    0.00   urn:smpte:ul:060E2B34.0401010D.03020104.00000000\n"
    "s   TSL      8   -90.00   60.00   http://dts.com/mda/channels#TSL\n"
    "s   TSR      9    90.00   60.00   http://dts.com/mda/channels#TSR\n"
    "s   LRS1    10  -150.00    0.00\n"
    "s   LRS2    11  -169.11    0.00\n"
    "s   LSS1    12   -45.00    0.00\n"
    "s   LSS2    13   -71.57    0.00\n"
    "s   LSS3    14  -108.43    0.00\n"
    "s   LSS4    15  -135.00    0.00\n"
    "s   RRS1    16   150.00    0.00\n"
    "s   RRS2    17   169.11    0.00\n"
    "s   RSS1    18    45.00    0.00\n"
    "s   RSS2    19    71.57    0.00\n"
    "s   RSS3    20   108.43    0.00\n"
    "s   RSS4    21   135.00    0.00\n"
    "s   TSL1    22   -24.79   35.99\n"
    "s   TSL2    23   -54.18   54.55\n"
    "s   TSL3    24  -125.82   54.55\n"
    "s   TSL4    25  -155.21   35.99\n"
    "s   TSR1    26    24.79   35.99\n"
    "s   TSR2    27    54.18   54.55\n"
    "s   TSR3    28   125.82   54.55\n"
    "s   TSR4    29   155.21   35.99\n"
    "w   LFE\n"
    "p   TSL4   TSL3   TSR4\n"
    "p   TSL4   TSL3   TSR3\n"
    "p   TSL4   TSL3   LSS4\n"
    "p   TSL4   LRS2   TSR4\n"
    "p   TSL4   LRS2   LRS1\n"
    "p   TSL4   LRS2   RRS2\n"
    "p   TSL4   TSR4   TSR3\n"
    "p   TSL4   TSR4   RRS2\n"
    "p   TSL4   LRS1   LSS4\n"
    "p   TSL3   TSL2   LSS3\n"
    "p   TSL3   TSL2   LSS2\n"
    "p   TSL3   TSL2   TSR3\n"
    "p   TSL3   TSL2   TSR2\n"
    "p   TSL3   LSS3   LSS2\n"
    "p   TSL3   LSS3   LSS4\n"
    "p   TSL3   TSR4   TSR3\n"
    "p   TSL3   TSR3   TSR2\n"
    "p   TSL2   TSL1   LSS1\n"
    "p   TSL2   TSL1   TSR2\n"
    "p   TSL2   TSL1   TSR1\n"
    "p   TSL2   LSS3   LSS2\n"
    "p   TSL2   LSS2   LSS1\n"
    "p   TSL2   TSR3   TSR2\n"
    "p   TSL2   TSR2   TSR1\n"
    "p   C      TSL1   L\n"
    "p   C      TSL1   TSR1\n"
    "p   C      R      TSR1\n"
    "p   TSL1   LSS1   L\n"
    "p   TSL1   TSR2   TSR1\n"
    "p   RSS4   TSR4   TSR3\n"
    "p   RSS4   TSR4   RRS1\n"
    "p   RSS4   RSS3   TSR3\n"
    "p   RSS2   RSS3   TSR3\n"
    "p   RSS2   RSS3   TSR2\n"
    "p   RSS2   TSR3   TSR2\n"
    "p   RSS2   TSR2   RSS1\n"
    "p   LRS2   TSR4   RRS2\n"
    "p   TSR4   RRS1   RRS2\n"
    "p   RSS3   TSR3   TSR2\n"
    "p   R      RSS1   TSR1\n"
    "p   TSR2   RSS1   TSR1\n";


#endif /* TESTCONFIFG_H_ */
