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
//  UTF8_Test.cpp
//  bsTest_Pause
//  Created by Ton Kalker on 6/16/14.
//
//

#include "gtest/gtest.h"
#include "commonstream/utils/StringUtils.h"

using namespace CommonStream;

TEST(UTF8Test,UTF8TestCase) {
    
	std::wstring chinese(L"好久不见");
	
	std::wstring french(L"liberté");
	
    std::string cstr = StringUtils::wtoutf8(chinese);
	
    std::string fstr = StringUtils::wtoutf8(french);
	
//	std::cout << "UTF-8 : French : ";
//	
//	for (char c : fstr)
//		
//		std::cout  << '[' << std::hex << int(static_cast<unsigned char>(c)) << ']';
//	
//	std::cout << '\n';
//	
//	std::cout << "UTF-8 : Chinese : ";
//	
//	for (char c : cstr)
//		
//		std::cout  << '[' << std::hex << int(static_cast<unsigned char>(c)) << ']';
//	
//	std::cout << '\n';
	
    std::wstring wcstr = StringUtils::utf8tow(cstr);
	
	EXPECT_EQ(chinese,wcstr);
	
    std::wstring wfstr = StringUtils::utf8tow(fstr);

	EXPECT_EQ(french,wfstr);
}
