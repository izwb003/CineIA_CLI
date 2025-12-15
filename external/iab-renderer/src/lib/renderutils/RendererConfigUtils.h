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


#ifndef __RENDERERCONFIGUTILIS_H__
#define	__RENDERERCONFIGUTILIS_H__

#include <iostream>
#include <vector>
#include <sstream>
#include <stdint.h>

namespace RenderUtils
{

    template <class T>
    std::vector<T> tokenize(T str, T dlm)
    {

        std::vector<T> result;

        int32_t epos = 0;
        int32_t bpos = 0;

        T token;

        bpos = (int32_t) str.find_first_not_of(dlm,epos);

        while ((bpos != T::npos) && (epos != T::npos))
        {

            epos = (int32_t) str.find_first_of(dlm,bpos);

            if (epos != T::npos)
            {

                token = str.substr(bpos,epos-bpos);

                bpos = (int32_t) str.find_first_not_of(dlm,epos);


            }
            else
            {

                token = str.substr(bpos);

            }

            result.push_back(token);

        }

        return result;

    }

    template <class T>
    std::vector<T> linize(T str)
    {

        return tokenize<T>(str,"\n\r");

    }

    template<class T>
    std::vector<T> tokenize(T str)
    {

        return tokenize<T>(str,"\t\n ");
    }



// readnumerical() is a template function to read numerical values from string tokens.
// Note: template type T must be numerical.

    template<class T>
    bool readnumerical(std::string s, T &rval)
    {

        std::stringstream ss;

        ss << s;

        ss >> rval;

        if (ss.fail())
        {

            rval = 0;

            return false;
        }
        else
        {

            return true;
        }
    }


    /*
     * Usage
     *
     * std::string str = " 1, 34, 44 ,,, 731,,,,, 5 \n tt ss ,, 3r";
     *
     * std::vector<std::wstring> lns = linize(str);
     *
     * for (int i = 0; i < lns.size();i++) {
     *
     *   tokenize(lns[i]);
     *
     * }
     */


} // RenderUtils


#endif /* defined(__RENDERERCONFIGUTILIS_H__) */
