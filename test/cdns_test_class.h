/*
* Author: Christian Huitema
* Copyright (c) 2018, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef cdns_test_H
#define cdns_test_H
#include <stdio.h>
#include <stdint.h>

#ifdef _cplusplus
#if _cplusplus < 199711L
#ifndef override
#define override 
#endif
#endif
#endif


void SET_LOG_FILE(FILE* f_log);
FILE*  GET_LOG_FILE();
void TEST_LOG(const char * fmt, ...);

class cdns_test_class
{
public:
    cdns_test_class();
    virtual ~cdns_test_class();

    virtual bool DoTest() = 0;

    static int get_number_of_tests();
    static char const * GetTestName(int number);
    static int GetTestNumberByName(const char * name);
    static cdns_test_class * TestByNumber(int number);
};

#endif /* cdns_test_H */