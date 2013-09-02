/*  CaveArt is a platform-independent image editing tool. */
/*  It makes use of GLFW, FreeImage, Boost, and open      */
/*  source libraries that use the GPL license or          */
/*  otherwise are open source and available for commercial*/
/*  use.It also makes use of a small header-only          */
/*  library called Min-GUI, created by Kiel Geiger.       */
/*                                                        */
/*  Contributors so far include Kiel Geiger.              */

#include "Min-GUI.hpp"
#include "GUI_Utils.h"
#include "Pixels.h"

int main(int argc, char** argv) {
    GLFWwindow* main_win;
    if(!glfwInit()) {
        return -1;
    }

    main_win = glfwCreateWindow(500, 500, "FreeArt", NULL, NULL);
    if(!main_win) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(main_win);
    while(!glfwWindowShouldClose(main_win)) {
        //do rendering stuff
        glfwSwapBuffers(main_win);
        glfwPollEvents();
    }
    glfwDestroyWindow(main_win);
    glfwTerminate();

    return 0;
}
