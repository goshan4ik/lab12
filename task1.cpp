#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
//! Переменные с индентификаторами ID
//! ID шейдерной программы
GLuint Program;
//! ID юниформ переменной цвета
GLint Unif_color;
double rotate_z = 0;
//! Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
void checkOpenGLerror() {
    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

GLint bindUnif(const char* unif_name) {
    auto code = glGetUniformLocation(Program, unif_name);
    if (code == -1)
    {
        std::cout << "could not bind uniform " << unif_name << std::endl;
    }
    return code;
}

//! Инициализация шейдеров
void initShader()
{
    //! Исходный код шейдеров
    const char* fsSource =
            "uniform vec2 u_resolution;\n"
            "void main() {\n"
            " vec2 st = gl_FragCoord.xy/800;\n"
            " vec3 color1 = vec3(1.9,0.55,0);\n"
            " vec3 color2 = vec3(0.226,0.000,0.615);\n"
            " float mixValue = distance(st,vec2(0,1));\n"
            " vec3 color = mix(color1,color2,mixValue);\n"
            " gl_FragColor = vec4(color, mixValue);\n"
            "}\n";
    //! Переменные для хранения идентификаторов шейдеров
    GLuint fShader;
    //! Создаем фрагментный шейдер
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    //! Передаем исходный код
    glShaderSource(fShader, 1, &fsSource, NULL);
    //! Компилируем шейдер
    glCompileShader(fShader);
    //! Создаем программу и прикрепляем шейдеры к ней
    Program = glCreateProgram();
    glAttachShader(Program, fShader);
    //! Линкуем шейдерную программу
    glLinkProgram(Program);
    //! Проверяем статус сборки
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        std::cout << "error attach shaders \n";
        return;
    }
    //! Вытягиваем ID юниформ
    bindUnif("u_resolution");

    checkOpenGLerror();
}
//! Освобождение шейдеров
void freeShader()
{
    //! Передавая ноль, мы отключаем шейдрную программу
    glUseProgram(0);
    //! Удаляем шейдерную программу
    glDeleteProgram(Program);
}
void resizeWindow(int width, int height)
{
    glViewport(0, 0, width, height);
}
//! Отрисовка
void render2()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glRotatef(rotate_z, 0.0, 0.0, 1.0);
    //! Устанавливаем шейдерную программу текущей
    glUseProgram(Program);
    static float red[2] = { 800.0f, 800.0f};
    //! Передаем юниформ в шейдер
    glUniform4fv(Unif_color, 1, red);
    glBegin(GL_TRIANGLES);
    /*
    -1.0f, -1.0f },
 { 0.0f, 1.0f },
 { 1.0f, -1.0f }
 glVertex2f(-0.5f, -0.5f);
     glVertex2f(0.0f, 0.5f);
     glVertex2f(0.5f, -0.5f);*/
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);
    glEnd();
    glFlush();
    //! Отключаем шейдерную программу
    glUseProgram(0);
    checkOpenGLerror();
    glutSwapBuffers();
}
void specialKeys(int key, int x, int y) {

    switch (key) {
        case GLUT_KEY_PAGE_UP: rotate_z += 5; break;
        case GLUT_KEY_PAGE_DOWN: rotate_z -= 5; break;
    }
    glutPostRedisplay();
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Simple shaders");
    glClearColor(0, 0, 0, 0);
    //! Обязательно перед инициализацией шейдеров
    GLenum glew_status = glewInit();
    if (GLEW_OK != glew_status)
    {
        //! GLEW не проинициализировалась
        std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";
        return 1;
    }
    //! Проверяем доступность OpenGL 2.0
    if (!GLEW_VERSION_2_0)
    {
        //! OpenGl 2.0 оказалась не доступна
        std::cout << "No support for OpenGL 2.0 found\n";
        return 1;
    }
    //! Инициализация шейдеров
    initShader();
    glutReshapeFunc(resizeWindow);
    glutDisplayFunc(render2);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    //! Освобождение ресурсов
    freeShader();
}

/*
 * "uniform vec2 u_resolution;\n"
        "void main() {\n"
        " vec2 st = gl_FragCoord.xy/800;\n"
        " vec3 color1 = vec3(1.9,0.55,0);\n"
        " vec3 color2 = vec3(0.226,0.000,0.615);\n"
        " float mixValue = distance(st,vec2(0,1));\n"
        " vec3 color = mix(color1,color2,mixValue);\n"
        " gl_FragColor = vec4(color, mixValue);\n"
        "}\n";
 * */