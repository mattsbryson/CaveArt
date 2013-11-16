#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include<queue>
#include<list>
#include<unordered_map>

#include "Types\CharcoalTypes.h"

/*
    For functions, I peeked into the Python source code
    a bit and borrowed some ideas from

    http://binf.gmu.edu/software/SWIG/Scripting.html

    and got a little inspiration from the C++ language itself.
*/

struct _chclCommand {
    _chclType _retType;
    VarTable locals;
    std::string name;
};

class Charcoal_Interpreter {        //Charcoal, get it? From CaveArt? Eh? Eh?
    std::list<_chclType> types;
    std::queue<_chclCommand> commands;
    VarTable globals;

    std::vector<unsigned int> getFunctionDeclLines();
    void include(std::string filename);

    Charcoal_Interpreter();
public:
    ~Charcoal_Interpreter();
    Charcoal_Interpreter(const Charcoal_Interpreter& intp) = delete;
    static Charcoal_Interpreter& getInterp();
    void eval();
};

#endif // INTERPRETER_H_INCLUDED
