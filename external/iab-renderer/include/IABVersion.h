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
 * Constants that contain the current IAB SDK version
 *
 * @file
 */

#ifndef __IABVERSION_H__
#define	__IABVERSION_H__

#define IAB_VERSION_MAJOR       1
#define IAB_VERSION_MINOR       2
#define IAB_VERSION_REVISION    1
#define IAB_VERSION_SUBREVISION 0

//
// https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
#define STRINGIFY2(s) #s
#define STRINGIFY(s) STRINGIFY2(s)
#define WIDE_STRINGIFY2(s) L ## #s
#define WIDE_STRINGIFY(s) WIDE_STRINGIFY2(s)

//
// The following have been verified with toolchain preprocessors 'gcc -E' on
// OSX/Linux and 'cl /P /C' on Windows
//
// produces the string form: "M.m.r.s"
#define IAB_VERSION_STRING \
    STRINGIFY(IAB_VERSION_MAJOR.IAB_VERSION_MINOR.IAB_VERSION_REVISION.IAB_VERSION_SUBREVISION)

// produces the string form: L"M.m.r.s"
#define IAB_VERSION_STRING_WIDE \
    WIDE_STRINGIFY(IAB_VERSION_MAJOR.IAB_VERSION_MINOR.IAB_VERSION_REVISION.IAB_VERSION_SUBREVISION)

/******************************************** for Windows .rc2 version resources ***********************/
#ifdef _WIN32

#ifdef _UNICODE
#define WIN_STRFILEVER IAB_VERSION_STRING_WIDE
#else
#define WIN_STRFILEVER IAB_VERSION_STRING
#endif // _UNICODE

#define WIN_STRPRODUCTVER  WIN_STRFILEVER
#define WIN_FILEVER        IAB_VERSION_MAJOR,IAB_VERSION_MINOR,IAB_VERSION_REVISION,IAB_VERSION_SUBREVISION
#define WIN_PRODUCTVER     WIN_FILEVER

#endif // _WIN32


#endif // __IABVERSION_H__
