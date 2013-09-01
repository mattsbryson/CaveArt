#ifndef MINGUI_HPP
#define MINGUI_HPP

#include <string>
#include <vector>
#include "boost/regex.hpp"
#include <typeinfo>

typedef void (*voidFunc)();

class BaseGUIObject {
    int xPos;  //Data to use for the actual button.
    int yPos;  //Will be put into OpenGL (or possibly
    int width;   //DirectX). The point (xPos, yPos)
    int height;  //is to be the bottom-left corner.

protected:

    BaseGUIObject(int x, int y, int wid, int hght) {
        xPos = x;
        yPos = y;
        width = wid;
        height = hght;
    }

public:

    void getCorners(int* coordPtr) {    //coordPtr = coordinate pointer. This function should be called
        coordPtr[0] = xPos;             //as 'int* butOneCornrs; butOne.getCorners(butOneCornrs);'
        coordPtr[1] = yPos;             //A version might be made that returns a std::array<int, 8>
        coordPtr[2] = (xPos + width);
        coordPtr[3] = yPos;             //Funny thing I didn't think about before, this setup lets someone
        coordPtr[4] = (xPos + width);   //use only one pointer, upload the information, then get another button in
        coordPtr[5] = yPos + height;    //and render that, etc. before swapping buffers.
        coordPtr[6] = xPos;
        coordPtr[7] = yPos + height;
    }

    inline int getX() { return xPos; }
    inline int getY() { return yPos; }
    inline int getWidth() { return width; }
    inline int getHeight() { return height; }
};

class BaseButton : public BaseGUIObject {
    bool held = false;    //To ensure that a button does not act until the mouse is released on top of it.
    bool clicked = false; //These are self-explanatory- a function polls mouse events, sets 'clicked' to true
    char* text;         //when the mouse is let go while on top of the button. Text goes on the button itself.

public:
    BaseButton(int x, int y, int wid, int hght, char* txt) : BaseGUIObject(x, y, wid, hght) {
        text = txt;
    }

    char* getCString() {    //For labelling purposes. I think I'll make an OpenGL implementation of character
        return text;        //rendering, but otherwise leave it to the dev(s) (more customization that way).
    }

    void setClicked(bool isLetGo) {     //"sets" (only for if the button is held down or released after being held
        if(held) {
            clicked = isLetGo;
        }
        else {
            clicked = false;
        }
    }

    void setHeld(bool hld) {
        if(!clicked) {
            held = hld;
        }
        else {
            held = false;
        }
    }

    inline bool isHeld() { return held; }       //"gets" (held down, released after held)
    inline bool isClicked() { return clicked; }
};

class RegButton : public BaseButton {
    void (*funcToExec)() = NULL;
public:
    RegButton(int x, int y, int wid, int hght, char* txt) : BaseButton(x, y, wid, hght, txt) {}

    inline void setFunc(voidFunc funcPtr = NULL) { funcToExec = funcPtr; } //funcToExec is NULL until this is called
                                                                      //calling with no arguments "disables" the button.
    inline void execFunc() {
        if(funcToExec != NULL) {
            (*funcToExec)();
        }
    }
};

class DrpDwnLst : public BaseButton {
    short numBtns = 0;
    char** buttonText;
    std::vector<voidFunc> FuncVec;      //vector of functions to be mapped to buttons
    std::vector<RegButton*> buttons;     //vector of buttons to be used

public:
    DrpDwnLst(int x, int y, int wid, int hght, char* txt, voidFunc* funcs, short nButns, char** btnTxt) : BaseButton(x, y, wid, hght, txt) {
        numBtns = nButns;
        buttonText = btnTxt;
        for(int i = 0; i < nButns; i++) {
            FuncVec.push_back((voidFunc) funcs[i]);     //defines values in FuncVec
        }
        buttons.reserve(nButns);        //reserves space for buttons but does not initialize any
    }

    ~DrpDwnLst() { for(int i = numBtns; i > 0; i--) { delete buttons[i-1]; } }

    void displayButtons() {
        for(int i = 0; i < numBtns; i++) {
            buttons[i] = new RegButton(getX(), (getY()*(i+1)), getWidth(), getHeight(), buttonText[i]); //constructs buttons
        }
        for(int i = 0; i < numBtns; i++) {
            buttons[i]->setFunc(FuncVec[i]);     //binds functions to them
        }
    }

    void hideButtons() {
        for(int i = numBtns; i > 0; i--) {
            buttons[i-1]->setFunc();     //frees function pointers
        }
        for(int i = numBtns; i > 0; i--) {
            delete buttons[i-1];   //destructs buttons
        }
    }

    void getPoints(int* pts, int indx) {
        buttons[indx]->getCorners(pts);      //gets values for buttons at a given index
    }

    void executeButtonFunc(int indx) {
        buttons[indx]->execFunc();       //executes the function bound to the button at a certain index
    }

    int getButtonIndex(int* mouseX, int* mouseY) {                  //calculates the index of the button that the use has their mouse over
        if(*mouseX > getX() && *mouseX < (getX() + getWidth())) {
            if(*mouseY > getY() && *mouseY + (getY() + (getHeight() * numBtns))) {
                return ((*mouseY / getHeight()) - 1);
            }
        }
    }
};

template <typename T> class IncDecButton : public BaseButton {
    T MAX_VALUE;    //for setting limits on the value of the variable
    T MIN_VALUE;
    T INCR_AMNT;    //custom increment amount
    T* valToChange; //keeps the address of the value to change, fine for copy c'tors
public:
    IncDecButton(int x, int y, int width, int height, T* val, T maxVal, T minVal, T incrAmnt) : BaseButton(x, y, width, height, "") {
        MAX_VALUE = maxVal;
        MIN_VALUE = minVal;
        INCR_AMNT = incrAmnt;
        valToChange = val;
    }

    void increment() {
        if(*valToChange < MAX_VALUE) {
            *valToChange += INCR_AMNT;
        }
    }

    void decrement() {
        if(*valToChange > MIN_VALUE) {
            *valToChange -= INCR_AMNT;
        }
    }
};

template <typename T> class Slider : public BaseGUIObject {
    T MAX_VALUE;
    T MIN_VALUE;
    T INCR_AMNT;
    T* valToChange;
public:
    Slider(T minVal, T maxVal, T incrAmnt, T* val, int x, int y, int wid, int high) : BaseGUIObject(x,y,wid,high) {
        MIN_VALUE = minVal;
        MAX_VALUE = maxVal;
        INCR_AMNT = incrAmnt;
        valToChange = val;
    }

    void changeVal(T offset) {
        *valToChange += offset;
    }

    T getOffset(T startPoint, T endPoint) {
        return (endPoint - startPoint);
    }
};

template <typename T> class InputBox : public BaseGUIObject {
    using type_value = T;

    typedef char (*charFunc)();
    typedef void (InputBox<T>::*strFunc)(std::string str);

    T* valToChange;
    charFunc input = NULL;
    std::string inStr;
    strFunc inptFunc = NULL;

    void strInpt(std::string str) {
        *valToChange = str.c_str();
    }

    void intInpt(std::string str) {
        boost::regex ints("[-]?[1-9][^.][0-9]*");
        boost::regex_replace(str, ints, "");
        int resInt = atoi(str.c_str());
        *valToChange = (type_value) resInt;  //in case type value is single char or any int variant
    }

    void floatInpt(std::string str) {
        boost::regex floats("[-]?[0-9]+(.)[0-9]*");
        boost::regex_replace(str, floats, "");
        float resFlt = atof(str.c_str());
        *valToChange = (type_value) resFlt;  //in case type_value is double
    }

public:
    InputBox(int x, int y, int width, int height, charFunc in, T* val) : BaseGUIObject(x, y ,width, height) {
        valToChange = val;
        input = in;
        inptFunc = &InputBox<T>::strInpt;
    }

    void read() {
        std::string inStr = "";
        while((*input)() != '\n') {
            inStr += (*input)();
        }
        (this->*inptFunc)(inStr);
    }
};

template <> class InputBox<int> : public BaseGUIObject {
    typedef char (*charFunc)();

    int* valToChange;
    charFunc input = NULL;
    std::string inStr;

    void intInpt(std::string str) {
        boost::regex ints("^[-]?[1-9][^.][0-9]*");
        boost::regex_replace(str, ints, "");
        int resInt = atoi(str.c_str());
        *valToChange = resInt;
    }

public:
    InputBox(int x, int y, int width, int height, charFunc in, int* val) : BaseGUIObject(x, y ,width, height) {
        valToChange = val;
        input = in;
    }

    void read() {
        std::string inStr = "";
        while((*input)() != '\n') {
            inStr += (*input)();
        }
        intInpt(inStr);
    }
};

template <> class InputBox<long long> : public BaseGUIObject {
    typedef char (*charFunc)();

    long long* valToChange;
    charFunc input = NULL;
    std::string inStr;

    void intInpt(std::string str) {
        boost::regex ints("^[-]?[1-9][^.][0-9]*");
        boost::regex_replace(str, ints, "");
        long long resInt = atoll(str.c_str());
        *valToChange = resInt;
    }

public:
    InputBox(int x, int y, int width, int height, charFunc in, long long* val) : BaseGUIObject(x, y ,width, height) {
        valToChange = val;
        input = in;
    }

    void read() {
        std::string inStr = "";
        while((*input)() != '\n') {
            inStr += (*input)();
        }
        intInpt(inStr);
    }
};

template <> class InputBox<short> : public BaseGUIObject {
    typedef char (*charFunc)();

    short* valToChange;
    charFunc input = NULL;
    std::string inStr;

    void intInpt(std::string str) {
        boost::regex ints("[-]?[1-9][^.][0-9]*");
        boost::regex_replace(str, ints, "");
        short resInt = (short) atoi(str.c_str());
        *valToChange = resInt;
    }

public:
    InputBox(int x, int y, int width, int height, charFunc in, short* val) : BaseGUIObject(x, y ,width, height) {
        valToChange = val;
        input = in;
    }

    void read() {
        std::string inStr = "";
        while((*input)() != '\n') {
            inStr += (*input)();
        }
        intInpt(inStr);
    }
};

template <> class InputBox<float> : public BaseGUIObject {
    typedef char (*charFunc)();

    float* valToChange;
    charFunc input = NULL;
    std::string inStr;

    void floatInpt(std::string str) {
        boost::regex floats("[-]?[0-9]*(.)[0-9]*");
        boost::regex_replace(str, floats, "");
        float resFlt = atof(str.c_str());
        *valToChange = resFlt;  //in case type_value is double
    }

public:
    InputBox(int x, int y, int width, int height, charFunc in, float* val) : BaseGUIObject(x, y ,width, height) {
        valToChange = val;
        input = in;
    }

    void read() {
        std::string inStr = "";
        while((*input)() != '\n') {
            inStr += (*input)();
        }
        floatInpt(inStr);
    }
};

template <> class InputBox<double> : public BaseGUIObject {
    typedef char (*charFunc)();

    double* valToChange;
    charFunc input = NULL;
    std::string inStr;

    void doubleInpt(std::string str) {
        boost::regex floats("[-]?[0-9]*(.)[0-9]*");
        boost::regex_replace(str, floats, "");
        double resDbl = atof(str.c_str());
        *valToChange = resDbl;
    }

public:
    InputBox(int x, int y, int width, int height, charFunc in, double* val) : BaseGUIObject(x, y ,width, height) {
        valToChange = val;
        input = in;
    }

    void read() {
        std::string inStr = "";
        while((*input)() != '\n') {
            inStr += (*input)();
        }
        doubleInpt(inStr);
    }
};

#endif // MINGUI_H
