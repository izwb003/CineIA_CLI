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

 /**
  * Instantiate a streambuf from an iterator
  *
  * @file
  */

#ifndef COMMON_STREAM_SRC_UTILS_IITERSBUF_H_
#define COMMON_STREAM_SRC_UTILS_IITERSBUF_H_

#include <streambuf>
#include <string>
#include "commonstream/utils/Namespace.h"

namespace CommonStream
{

template<class I>
class iitersbuf : public std::streambuf
{
public:

    iitersbuf(I pStart, I pEnd);

    I& getIterator();

protected:

    virtual int_type underflow();

    virtual int_type uflow();

    I			fHead;
    I			fTail;
};

template<class I>
iitersbuf<I>::iitersbuf(I pStart,
                        I pEnd)
:
std::streambuf()
{
    fHead = pStart;
    fTail = pEnd;
}

template<class I>
I& iitersbuf<I>::getIterator()
{
    return fHead;
}

template<class I>
std::streambuf::int_type iitersbuf<I>::underflow()
{
    if (fHead != fTail)
    {
        return std::char_traits<char>::to_int_type(*fHead);
    }
    else
    {
        return (std::char_traits<char>::eof());
    }
}

template<class I>
std::streambuf::int_type iitersbuf<I>::uflow()
{
    std::streambuf::int_type r = underflow();

    if (r != std::char_traits<char>::eof())
    {
        fHead++;
    }

    return r;
}

}  // namespace CommonStream

#endif // COMMON_STREAM_SRC_UTILS_IITERSBUF_H_
