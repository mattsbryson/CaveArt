#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include "Min-GUI.hpp"
#include <GL/glfw3.h>

void correctGetMousePos(GLFWwindow* wnd, double* mX, double* mY) {
    int scrnW, scrnHt;
    glfwGetWindowSize(wnd, &scrnW, &scrnHt);
    glfwGetCursorPos(wnd, mX, mY);
    *mY = (scrnHt - *mY);
}

bool mouseOnObject(GLFWwindow* wnd, BaseGUIObject obj) {
    double mouseX, mouseY;
    correctGetMousePos(wnd, &mouseX, &mouseY);
    if((mouseX > obj.getX()) && (mouseX < (obj.getX() + obj.getWidth()))) {
        if((mouseY > obj.getY()) && (mouseY < (obj.getY() + obj.getHeight()))) {
            return true;
        }
    }
    return false;
}

bool is_held(GLFWwindow* wnd) {
    double mouseX, mouseY;
    correctGetMousePos(wnd, &mouseX, &mouseY);
    if(glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        return true;
    }
    return false;
}

bool is_released(GLFWwindow* wnd) {
    if(glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            return true;
    }
    return false;
}

#endif // GUI_UTILS_H
