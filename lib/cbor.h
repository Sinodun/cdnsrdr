/*
* Author: Christian Huitema
* Copyright (c) 2019, Private Octopus, Inc.
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

#ifndef CBOR_H
#define CBOR_H

#include <vector>

#define CBOR_CLASS(x) (((x)>>5)&7)
#define CBOR_T_UINT 0
#define CBOR_T_NINT 1
#define CBOR_T_BYTES 2
#define CBOR_T_TEXT 3
#define CBOR_T_ARRAY 4
#define CBOR_T_MAP 5
#define CBOR_T_TAGGED 6
#define CBOR_T_FLOAT 7

#define CBOR_END_OF_ARRAY -1
#define CBOR_ILLEGAL_VALUE -2
#define CBOR_MALFORMED_VALUE -3
#define CBOR_NOT_IMPLEMENTED -4
#define CBOR_UNEXPECTED -5
#define CBOR_MEMORY -6

#define CBOR_END_MARK 0xff


uint8_t* cbor_get_number(uint8_t* in, uint8_t const* in_max, int64_t* val);
char* cbor_print_int(char* out, char const* out_max, int64_t val, int is_negative);
char* cbor_print_text_part(char* out, char const* out_max, uint8_t* in, int64_t val);
char* cbor_print_bytes_part(char* out, char const* out_max, uint8_t* in, int64_t val);
uint8_t* cbor_text_to_text(uint8_t* in, uint8_t const* in_max, char** p_out, char const* out_max, int64_t val, int* err);
uint8_t* cbor_bytes_to_text(uint8_t* in, uint8_t const* in_max, char** p_out, char const* out_max, int64_t val, int* err);
uint8_t* cbor_float_to_text(uint8_t* in, uint8_t const* in_max, char** p_out, char const* out_max, int64_t val, int* err);
uint8_t* cbor_array_to_text(uint8_t* in, uint8_t const* in_max, char** p_out, char const* out_max, int64_t val, int is_map, int* err);
uint8_t* cbor_to_text(uint8_t* in, uint8_t const* in_max, char** p_out, char const* out_max, int* err);

uint8_t* cbor_skip(uint8_t* in, uint8_t const* in_max, int* err);

uint8_t* cbor_parse_int(uint8_t* in, uint8_t const* in_max, int* v, int is_signed, int* err);
uint8_t* cbor_parse_int64(uint8_t* in, uint8_t const* in_max, int64_t* v, int is_signed, int* err);
uint8_t* cbor_parse_boolean(uint8_t* in, uint8_t const* in_max, bool *v, int* err);

class cbor_bytes {
public:
    cbor_bytes();
    cbor_bytes(const cbor_bytes &other);
    ~cbor_bytes();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    uint8_t* v;
    size_t l;
};

class cbor_text {
public:
    cbor_text();
    cbor_text(const cbor_text& other);
    ~cbor_text();

    uint8_t* parse(uint8_t* in, uint8_t const* in_max, int* err);

    char* v;
    size_t l;
};

template <class ParsedClass> uint8_t* cbor_object_parse(uint8_t* in, uint8_t const* in_max, ParsedClass* v, int* err)
{
    in = v->parse(in, in_max, err);
    return in;
}

template <class ParsedClass, class CtxClass> uint8_t* cbor_object_ctx_parse(uint8_t* in, uint8_t const* in_max, ParsedClass* v, int* err, CtxClass* ctx)
{
    in = v->parse(in, in_max, err, ctx);
    return in;
}

uint8_t* cbor_object_parse(uint8_t* in, uint8_t const* in_max, int* v, int* err);

/* cbor_array_parse:
   Parse a CBOR input into an array of InnerType.
   This construct assumes that the Inner Class is copy insertable, i.e. has copy constructor,
   and that it can be parsed using cbor_objest_parse.
   If the generic cbor_object_parse<InnerClass> does not work, the implementation
   must supply a specific function, as in the integer example above.
   */
template <class InnerClass>
uint8_t* cbor_array_parse(uint8_t* in, uint8_t const* in_max, std::vector<InnerClass> * v, int* err)
{
    int64_t val;
    int outer_type = CBOR_CLASS(*in);
    int is_undef = 0;

    in = cbor_get_number(in, in_max, &val);

    if (in == NULL || outer_type != CBOR_T_ARRAY) {
        *err = CBOR_MALFORMED_VALUE;
        in = NULL;
    }
    else {
        int rank = 0;
        if (val == CBOR_END_OF_ARRAY) {
            is_undef = 1;
            val = 0xffffffff;
        }
        
        while (rank < val && in != NULL && in < in_max) {
            if (*in == 0xff) {
                if (is_undef) {
                    in++;
                }
                else {
                    *err = CBOR_MALFORMED_VALUE;
                    in = NULL;
                }
                break;
            }
            else {
                v->resize((size_t)rank + 1); 
                in = cbor_object_parse(in, in_max, &(*v)[rank], err);
                rank++;
            }
        }
    }

    return in;
}

/* cbor_ctx_array_parse:
   same as cbor_array_parse, but also pass an additional context parameter
   */
template <class InnerClass, class CtxClass>
uint8_t* cbor_ctx_array_parse(uint8_t* in, uint8_t const* in_max, std::vector<InnerClass>* v, int* err, CtxClass* ctx)
{
    int64_t val;
    int outer_type = CBOR_CLASS(*in);
    int is_undef = 0;

    in = cbor_get_number(in, in_max, &val);

    if (in == NULL || outer_type != CBOR_T_ARRAY) {
        *err = CBOR_MALFORMED_VALUE;
        in = NULL;
    }
    else {
        int rank = 0;
        if (val == CBOR_END_OF_ARRAY) {
            is_undef = 1;
            val = 0xffffffff;
        }

        while (rank < val && in != NULL && in < in_max) {
            if (*in == 0xff) {
                if (is_undef) {
                    in++;
                }
                else {
                    *err = CBOR_MALFORMED_VALUE;
                    in = NULL;
                }
                break;
            }
            else {
                v->resize((size_t)rank + 1);
                in = cbor_object_ctx_parse(in, in_max, &(*v)[rank], err, ctx);
                rank++;
            }
        }
    }

    return in;
}

/* cbor_map_parse: 
   Parse a CBOR input into a map element, in which each index is an integer.
   This construct assumes that the InnerClass has a method:
   uint8_t * parse_map_item(uint8_t *in, uint8_t const* in_max, int64_t index, int * err);
   The method is called for each index that is present, and shall parse the
   corresponding index.
*/
template <class InnerClass>
uint8_t* cbor_map_parse(uint8_t* in, uint8_t const* in_max, InnerClass * v, int* err)
{

    int outer_type = CBOR_CLASS(*in);
    int64_t val;
    int is_undef = 0;

    in = cbor_get_number(in, in_max, &val);

    if (in == NULL || outer_type != CBOR_T_MAP) {
        *err = CBOR_MALFORMED_VALUE;
        in = NULL;
    }
    else {
        if (val == CBOR_END_OF_ARRAY) {
            is_undef = 1;
            val = 0xffffffff;
        }

        while (val > 0 && in != NULL && in < in_max) {
            if (*in == 0xff) {
                if (is_undef) {
                    in++;
                }
                else {
                    *err = CBOR_MALFORMED_VALUE;
                    in = NULL;
                }
                break;
            }
            else {
                /* There should be two elements for each map item */
                int inner_type = CBOR_CLASS(*in);
                int64_t inner_val;

                in = cbor_get_number(in, in_max, &inner_val);
                if (in == NULL || (inner_type != CBOR_T_UINT && inner_type != CBOR_T_NINT)) {
                    *err = CBOR_MALFORMED_VALUE;
                    in = NULL;
                }
                else {
                    if (inner_type == CBOR_T_NINT) {
                        inner_val = -(inner_val + 1);
                    }
                    in = v->parse_map_item(in, in_max, inner_val, err);
                    val--;
                }
            }
        }
    }

    return in;
}

#endif
