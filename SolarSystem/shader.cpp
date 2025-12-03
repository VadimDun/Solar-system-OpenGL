#include "shader.h"

Assignment currentAssignment = ASSIGNMENT_1;

void ShaderLog(unsigned int shader) {
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1) {
        int charsWritten = 0;
        std::vector<char> infoLog(infologLen);
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
        std::cout << "InfoLog: " << infoLog.data() << std::endl;
    }
}

void checkOpenGLerror() {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

void Init() {
    switch (currentAssignment) {
        case ASSIGNMENT_1:
            Tetrahedron_InitShader();
            Tetrahedron_InitVBO();
            break;
        case ASSIGNMENT_2:
            //Cube1_InitShader();
            //Cube1_InitVBO();
            break;
        case ASSIGNMENT_3:
            //Cube2_InitShader();
            //Cube2_InitVBO();
            break;
        case ASSIGNMENT_4:
            Circle_InitShader();
            Circle_InitVBO();
            break;
    }
    glEnable(GL_DEPTH_TEST);
}

void Draw() {
    switch (currentAssignment) {
        case ASSIGNMENT_1:
            Tetrahedron_Draw();
            break;
        case ASSIGNMENT_2:
            //Cube1_Draw();
            break;
        case ASSIGNMENT_3:
            //Cube2_Draw();
            break;
        case ASSIGNMENT_4:
            Circle_Draw(); 
            break;
    }
}

void Release() {
    switch (currentAssignment) {
        case ASSIGNMENT_1:
            Tetrahedron_Release();
            break;
        case ASSIGNMENT_2:
            //Cube1_Release();
            break;
        case ASSIGNMENT_3:
            //Cube2_Release();
            break;
        case ASSIGNMENT_4:
            Circle_Release();
            break;
    }
}
