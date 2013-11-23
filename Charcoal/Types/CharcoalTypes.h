#ifndef CHARCOALTYPES_H_INCLUDED
#define CHARCOALTYPES_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <climits>
#include <typeinfo>

#include <string>
#include <vector>


/*
    The following code is inspired by Vijay Matthew's post
    on Stack Overflow concerning a type system with variables
    implemented in C++ using a string-type map.

    http://stackoverflow.com/a/2697159/2620007

    This is meant to be a "header-only library".
*/

struct _chclType;
struct Bool;
struct Void;
struct Int;
struct Float;
struct Struct;

struct _chclType {
    typedef std::vector<_chclType*> typeVec;  //allows for interations between multiple different types

    virtual ~_chclType() {};  //defined for each type

    virtual operator std::string() const = 0; //instead of a toString method, a std::string casting operator

    virtual bool operator ==(const _chclType* other) = 0; //regular equality operators
    virtual bool operator !=(const _chclType* other) = 0;

    virtual std::string type() const = 0;

    virtual _chclType* invoke_op(const std::string& oper, const typeVec& args) = 0;
};

struct Bool : _chclType {
private:
    _chclType* _ret = nullptr;
public:
    bool tf;

    virtual operator std::string() const {
        if(tf) {
            return "true";
        } else {
            return "false";
        }
    }

    virtual std::string type() const {
        return "Bool";
    }

    virtual bool operator==(const _chclType* other) {
        if(other->type() == this->type()) {
            try {
                const Bool* temp = dynamic_cast<const Bool*> (other);
                if(temp->tf == this->tf) {
                    return true;
                } else {
                    return false;
                }
            } catch(std::bad_cast ex) {
                return false;
            }
        } else {
            return false;
        }
    }

    virtual bool operator!=(const _chclType* other) {
        return !(*this == other);
    }

    virtual bool operator!() {
        return !tf;
    }

    Bool(bool b) : tf(b) {}
    ~Bool() {
        if(_ret != nullptr) {
            delete _ret;
        }
    }

    Bool& operator=(bool b) {
        tf = b;
        return (Bool&)(*this);
    }

    explicit operator bool() {
        return tf;
    }

    virtual _chclType* invoke_op(const std::string& oper, const typeVec& args) {
        if((oper == "==") || (oper == "equal to")) {
            _ret = new Bool((*this == args[1]));
            return _ret;
        } else if((oper == "!=") || (oper == "not equal to")) {
            _ret = new Bool((*this != args[1]));
            return _ret;
        } else if((oper == "!") || (oper == "not")){
            _ret = new Bool(!(*this));
            return _ret;
        } else if((oper == "&&") || (oper == "and")){
            _ret = new Bool((tf && args[1]));
            return _ret;
        } else if((oper == "||") || (oper == "or")) {
            _ret = new Bool((tf || args[1]));
            return _ret;
        } else {
            return _ret;
        }
    }
};

struct Void : _chclType {
private:
    _chclType* _ret = nullptr;
public:

    virtual operator std::string() const {
        return std::string("VOID");
    }

    virtual std::string type() const {
        return "VOID";
    }

    virtual bool operator==(const _chclType& other) {
        if(other.type() == this->type()) {
            return true;
        } else {
            return false;
        }
    }

    virtual bool operator!=(const _chclType& other) {
        return !(*this == other);
    }

    virtual _chclType* invoke_op(const std::string& oper, const typeVec& args) {
        if(oper== "=") {
            return (_chclType*) NULL;
        } else if((oper == "==") || (oper == "equal to")) {
            _ret = new Bool(args[1] == NULL);
            return _ret;
        } else if ((oper == "!=") || (oper == "not equal to")) {
            _ret = new Bool(args[1] != NULL);
            return _ret;
        } else {
            return (_chclType*) NULL;
        }
    }
};



struct Int : _chclType {
private:
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
    } uni_int;              //sizeof(uni_int) == 8 because sizeof(signed_int) == sizeof(usigned_int) Uni-int in 64 bits

    _chclType* _ret = nullptr;
public:
    uni_int val;
    bool sign;

    virtual operator std::string() const {
        std::string toStr = "";
        if(sign) {
            toStr += val.si._64b;
        } else {
            toStr += val.usi._u64b;
        }
        return toStr;
    }

    virtual std::string type() const {
        return "Int";
    }

    virtual bool operator==(const _chclType* other) {
        if(other->type() == "Int") {
            try {
                const Int* temp = dynamic_cast<const Int*> (other);
                if(temp->sign) {
                    if(temp->val.si._64b == val.si._64b) {
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    if(temp->val.usi._u64b == val.usi._u64b) {
                        return true;
                    } else {
                        return false;
                    }
                }
            }
            catch (std::bad_cast ex)
            {
                return false;
            }
        } else {
            return false;
        }
    }

    virtual bool operator!=(const _chclType* other) {
        return !(*this == other);
    }

    Int() {
        val.si._64b = 0;
    }

    //defining constructors for only int32_t, int64_t, uint32_t, and uint64_t

    Int(const int32_t i) {
        val.si._32b = i;
    }

    Int(int64_t i) {
        val.si._64b = i;
    }

    Int(const uint32_t i) {
        val.usi._u32b = i;
    }

    Int(uint64_t i) {
        val.usi._u64b = i;
    }

    Int& operator=(const int32_t i) {
        val.si._32b = i;
        return (Int&)(*this);
    }

    Int& operator=(const int64_t i) {
        val.si._64b = i;
        return (Int&)(*this);
    }

    Int& operator=(const uint32_t i) {
        val.usi._u32b = i;
        return (Int&)(*this);
    }

    Int& operator=(const uint64_t i) {
        val.usi._u64b = i;
        return (Int&)(*this);
    }

    Int& operator=(const Int& I) {
        *this = I;
        return (Int&)(*this);
    }

    Int operator+(const int64_t& i) {        //addition with integer literals
        if(i < 0) {
            return (*this - (-i));
        } else {
            if((LLONG_MAX - val.si._64b) > i) {
                Int temp(val.si._64b + i);
                return temp;
            } else {
                Int temp(LLONG_MAX);
                return temp;
            }
        }
    }

    Int operator+(const uint64_t& i) {
        if((ULLONG_MAX - val.usi._u64b) > i) {
            Int temp(val.usi._u64b + i);
            return temp;
        } else {
            Int temp(ULLONG_MAX);
            return temp;
        }
    }


    Int operator-(const int64_t& i) {
        if(i < 0) {
            return (*this + (-i));
        } else {
            if( ((LLONG_MIN + std::abs(val.si._64b)) > (LLONG_MIN + i) )) {
                Int temp = val.si._64b - i;
                return temp;
            } else {
                Int temp(LLONG_MIN);
                return temp;
            }
        }
    }

    Int operator-(const uint64_t& i) {
        if((val.usi._u64b - i) > 0) {
            Int temp(val.usi._u64b - i);
            return temp;
        } else {
            Int temp(0);
            return temp;
        }
    }

    Int operator*(const int64_t& i) {
        if((i < 0) != (val.si._64b < 0)) {
            if(std::abs(LLONG_MIN / val.si._64b) > std::abs(i)) {
                Int temp(val.si._64b * i);
                return temp;
            } else {
                return Int(LLONG_MIN);
            }
        } else {
            if((LLONG_MAX / val.si._64b) > i) {
                Int temp(val.si._64b * i);
                return temp;
            } else {
                return Int(LLONG_MAX);
            }
        }
    }

    Int operator*(const uint64_t& i) {
        if((LLONG_MAX / val.usi._u64b) > i) {
            Int temp(val.usi._u64b * i);
            return temp;
        } else {
            return Int(LLONG_MAX);
        }
    }

    Int operator/(const int64_t& i) {
        return Int(val.si._64b / i);
    }

    Int operator/(const uint64_t& i) {
        return Int(val.usi._u64b / i);
    }

    virtual _chclType* invoke_op(const std::string& oper, const typeVec& args) {

    }
};

typedef std::unordered_map<std::string, _chclType*> VarTable;

#endif // CHARCOALTYPES_H_INCLUDED
