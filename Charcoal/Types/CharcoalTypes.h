#ifndef CHARCOALTYPES_H_INCLUDED
#define CHARCOALTYPES_H_INCLUDED

#include<string>
#include<vector>

/*
    The following code is inspired by Vijay Matthew's post
    on Stack Overflow concerning a type system with variables
    implemented in C++ using a string-type map.

    http://stackoverflow.com/a/2697159/2620007
*/

struct _chclType {
    typedef std::vector<_chclType*> typeVec;  //allows for interations between multiple different types

    virtual ~_chclType() {};  //defined for each type

    virtual operator std::string() const = delete;           //instead of a toString method, a std::string casting operator

    virtual bool operator ==(const _chclType& other) = delete; //regular equality operators
    virtual bool operator !=(const _chclType& other) = delete;

    virtual _chclType& invoke_op(const std::string& oper, const typeVec& args) = delete;
};

typedef std::unordered_map<std::string, _chclType> VarTable;

#endif // CHARCOALTYPES_H_INCLUDED
