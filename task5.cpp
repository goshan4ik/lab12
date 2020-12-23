#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

using namespace std;


//! Переменные с индентификаторами ID
//! ID шейдерной программы
GLuint Program;
//! ID атрибута
GLint Attrib_vertex;
//! ID юниформ переменной матрицы
GLint Unif_matrix;
//ID юниформ цветов
GLint Attrib_color;
//! ID Vertex Buffer Object
GLuint VBO_vertex;
//! ID Vertex Buffer Object
GLuint VBO_color;
//! ID Vertex Buffer Object
GLuint VBO_element;
//Количество индексов
GLint Indices_count;
//Матрица проекции
mat4 matrix_projection;
//Угол
double angle = 0;
//! Вершина
struct vertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
//! Функция печати лога шейдера
void shaderLog(unsigned int shader)
{
	int infologLen = 0;
	int charsWritten = 0;
	char* infoLog;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate InfoLog buffer\n";
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		std::cout << "InfoLog: " << infoLog << "\n\n\n";
		delete[] infoLog;
	}
}
//! Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
void checkOpenGLerror()
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}
//! Инициализация шейдеров
void initShader()
{
	//! Исходный код шейдеров
	const GLchar* vsSource =
		"#version 330 core\n"
		"attribute vec3 coord;\n"
		"attribute vec3 color;\n"
		"varying vec3 var_color;\n"
		"uniform mat4 matrix;\n"
		"void main() {\n"
		" gl_Position = matrix * vec4(coord, 1.0);\n"
		"var_color = color;\n"
		"}\n";
	const GLchar* fsSource =
		"#version 330 core\n"
		"varying vec3 var_color;\n"
		"void main() {\n"
		" gl_FragColor = vec4(var_color, 1.0);\n"
		"}\n";

	//! Переменные для хранения идентификаторов шейдеров
	GLuint vShader, fShader;
	//! Создаем вершинный шейдер
	vShader = glCreateShader(GL_VERTEX_SHADER);
	//! Передаем исходный код
	glShaderSource(vShader, 1, &vsSource, NULL);
	//! Компилируем шейдер
	glCompileShader(vShader);

	std::cout << "vertex shader \n";
	shaderLog(vShader);

	//! Создаем фрагментный шейдер
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//! Передаем исходный код
	glShaderSource(fShader, 1, &fsSource, NULL);
	//! Компилируем шейдер
	glCompileShader(fShader);
	std::cout << "fragment shader \n";
	shaderLog(fShader);

	//! Создаем программу и прикрепляем шейдеры к ней
	Program = glCreateProgram();
	glAttachShader(Program, vShader);
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

	//! Вытягиваем ID атрибута из собранной программы
	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl;
		return;
	}

	attr_name = "color";
	Attrib_color = glGetAttribLocation(Program, attr_name);
	//! Вытягиваем ID цвета
	if (Attrib_color == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl;
		return;
	}
	//! Вытягиваем ID матрицы
	attr_name = "matrix";
	Unif_matrix = glGetUniformLocation(Program, attr_name);

	checkOpenGLerror();
}

//! Инициализация VBO
void initVBO()
{
	//! Вершины нашего треугольника
	vertex vertices[] = {
	-1.0f , -1.0f , -1.0f ,
	 1.0f , -1.0f , -1.0f ,
	 1.0f , 1.0f , -1.0f ,
	-1.0f , 1.0f , -1.0f ,
	-1.0f , -1.0f , 1.0f ,
	 1.0f , -1.0f , 1.0f ,
	 1.0f , 1.0f , 1.0f ,
	-1.0f , 1.0f , 1.0f
	};

	//Цвета вершин
	vertex colors[] = {	
		0.0f,  0.980f,  0.604f,
		0.486f, 0.988f, 0.0f,
		0.863f,  0.078f,  0.235f,
		0.545f, 0.0f, 0.0f,
		0.957f, 0.643f, 0.376f,
		0.753f, 0.753f, 0.753f,
		0.184f, 0.310f, 0.310f,
		0.502f, 0.0f, 0.502f,
	};

	//Индексы
	GLint indices[] = {
		0 , 4 , 5 , 
		0 , 5 , 1 ,
		1 , 5 , 6 ,
		1 , 6 , 2 ,
		2 , 6 , 7 ,
		2 , 7 , 3 ,
		3 , 7 , 4 , 
		3 , 4 , 0 ,
		4 , 7 , 6 , 
		4 , 6 , 5 ,
		3 , 0 , 1 , 
		3 , 1 , 2
	};
	Indices_count = sizeof(indices) / sizeof(indices[0]);

	//! Передаем вершины в буфер
	glGenBuffers(1, &VBO_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//! Передаем цвета в буфер
	glGenBuffers(1, &VBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	//! Передаем индексы в буфер
	glGenBuffers(1, &VBO_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
//! Освобождение буфера
void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO_vertex);
	glDeleteBuffers(1, &VBO_element);
	glDeleteBuffers(1, &VBO_color);
}
void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}
//! Отрисовка
void render()
{
	//angle += 0.00005;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	mat4 View = lookAt(vec3(4, 3, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 rotate_x = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, glm::cos(angle), -glm::sin(angle), 0.0f,
		0.0f, glm::sin(angle), glm::cos(angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	matrix_projection = Projection * View * rotate_x;

	//! Устанавливаем шейдерную программу текущей
	glUseProgram(Program);
	//! Передаем юниформ в шейдер
	glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, &matrix_projection[0][0]);

	//setUniform(Unif_matrix, matrix_projection);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);
	//! Включаем массив атрибутов
	glEnableVertexAttribArray(Attrib_vertex);
	//! Подключаем VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	//! Указывая pointer 0 при подключенном буфере, мы указываем что данные в VBO
	glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(Attrib_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//! Отключаем VBO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawElements(GL_TRIANGLES, Indices_count, GL_UNSIGNED_INT, 0);
	//! Отключаем массив атрибутов
	glDisableVertexAttribArray(Attrib_vertex);
	glDisableVertexAttribArray(Attrib_color);
	//! Отключаем шейдерную программу
	glFlush();
	checkOpenGLerror();
	glutSwapBuffers();
	glUseProgram(0);
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Colored cube");
	//z-буфер
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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
	glClearColor(1.0, 1.0, 1.0, 0);

	//! Инициализация
	initVBO();
	initShader();
	glutReshapeFunc(resizeWindow);
	glutIdleFunc(render);
	glutDisplayFunc(render);
	glutMainLoop();
	//! Освобождение ресурсов
	freeShader();
	freeVBO();
}