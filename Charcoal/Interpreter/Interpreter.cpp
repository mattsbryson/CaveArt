#include "Interpreter.h"

Charcoal_Interpreter::Charcoal_Interpreter() {}

Charcoal_Interpreter& Charcoal_Interpreter::getInterp() {
    static Charcoal_Interpreter interpreter;
    return interpreter;
}

