#pragma once
#include <GL/glew.h>
#include <iostream>

namespace Debug
{
    inline void glErrorCheck(const char* file, int line) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            std::cout << errorNumber << " on line: " << line << " in file: " << file << std::endl;
            errorNumber = glGetError();
        }
    }
}

// TASK 3: Add a preprocessor directive to automate the writing of this function
#define glErrorCheck() glErrorCheck(__FILE__, __LINE__)
