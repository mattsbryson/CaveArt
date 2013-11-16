#ifndef CHARCOALTYPES_H_INCLUDED
#define CHARCOALTYPES_H_INCLUDED

#include<cstddef>
#include<cstdint>
#include<climits>

#include<string>
#include<vector>

/*
    The following code is inspired by Vijay Matthew's post
    on Stack Overflow concerning a type system with variables
    implemented in C++ using a string-type map.

    http://stackoverflow.com/a/2697159/2620007

    This is meant to be a "header-only library".
*/

struct _chclType {
    typedef std::vector<_chclType*> typeVec;  //allows for interations between multiple different types

    virtual ~_chclType() {};  //defined for each type

    virtual operator std::string() const = delete;           //instead of a toString method, a std::string casting operator

    virtual bool operator ==(const _chclType& other) = delete; //regular equality operators
    virtual bool operator !=(const _chclType& other) = delete;

    virtual _chclType& invoke_op(const std::string& oper, const typeVec& args) = delete;
};

struct Int : _chclType {
    typedef union {
        int8_t _8b;
        int16_t _16b;
        int32_t _32b;
        int64_t _64b;   //sizeof(signed_int) == 8 if 64-bit integers are available
    } signed_int;

    typedef union {
        uint8_t _u8b;
        uint16_t _u16b;
        uint32_t _u32b;
        uint64_t _u64b; //sizeof(usigned_int) == 8 if 64-bit integers are available
    } usigned_int;

    /*
    * I'm using all these types to make conversion a bit easier when it comes to pixel stuff-
    * if an Int is small enough to fit in an uint8_t and it's being used for a pixel channel
    * value, then it will be passed as an uint8_t (unsigned char) to a Pixel<8, PIXEL_FORMAT::fmt>
    * Also, if I'm going to be returning values to be stored elsewhere, it's actually more efficient
    * to have the different types here than have everything else be the highest possible bit-depth
    * to accomodate all possible sizes.
    */

    typedef union {
        signed_int si;
        usigned_int usi;
    } uni_int;              //sizeof(uni_int) == 16 Worth it?

    uni_int val;

    Int& operator =(const signed_int& i) {
        if(i._64b > SCHAR_MAX) {
            if(i._64b > SHRT_MAX) {
                if(i._64b > INT_MAX) {
                    if(i._64b > LLONG_MAX) {
                        goto error;         //I've committed a sin, but I do not repent!
                    } else {
                        val.si._64b = i._64b;
                    }
                } else {
                    val.si._32b = i._32b;
                }
            } else {
                val.si._16b = i._16b;
            }
        } else if(i._64b < SCHAR_MIN){           //this tree could be bundled with the other to lessen the space
            if(i._64b < SHRT_MIN) {              //but it's much clearer to read this way. If > SCHAR_MAX, if > SHRT_MAX...
                if(i._64b < INT_MIN) {           //else if < SCHAR_MIN, if < SHRT_MIN...
                    if(i._64b < LLONG_MIN) {
                        goto error;         //I've done it again! Are you disgusted yet?
                    } else {
                        val.si._64b = i._64b;
                    }
                } else {
                    val.si._32b = i._32b;
                }
            } else {
                val.si._16b = i._16b;
            }
        } else {
            val.si._8b = i._8b;
        }
        goto _end;

        error:      //this will probably never ever be reached unless there's some weird rule of C++ I don't understand
            val.si._64b = 0;    //if something is wrong, the largest bit depth member gets set to the default value
                                //I could give some debug output saying that the value is set to zero once I get the GUI
                                //interpreter running.
        _end:
            return (Int&)(*this);    //operator chaining yay!
    }
};

typedef std::unordered_map<std::string, _chclType> VarTable;

#endif // CHARCOALTYPES_H_INCLUDED
