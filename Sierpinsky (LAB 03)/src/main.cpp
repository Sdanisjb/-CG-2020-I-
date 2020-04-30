#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>

#define TRIANGLE_LEVEL 11

/*Colors Arrays*/
float colors[30] = {
	0.f, 0.f, 1.f, //Blue
	0.f, 1.f, 0.f, //Green
	1.f, 0.f, 0.f, //Red
	0.f, 0.5f, 0.5f, //Teal
	0.5f, 0.f, 0.5f, // Purple
	0.5f, 0.5f, 0.f, //Olive
	1.0f, 0.f, 1.0f, //Fucsia
	0.f, 1.f, 1.f, //Aqua
	1.f, 1.f, 0.f, //Yellow
	0.5f, 0.f, 0.f //Marron
};

int until = 1;
unsigned int startColor = 0;

/*Key Callback*/  
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_D && action == GLFW_PRESS) (until < TRIANGLE_LEVEL) ? until++ : until = TRIANGLE_LEVEL;
	if (key == GLFW_KEY_A && action == GLFW_PRESS) (until > 1) ? until-- : until = 1;
	if (key == GLFW_KEY_W && action == GLFW_PRESS) startColor = (startColor + 1) % 10;
	if (key == GLFW_KEY_S && action == GLFW_PRESS) startColor = (startColor - 1) % 10;
}

/*Shader Source Code*/

/*Vertex Shader*/
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

/*Fragment Shader: gets a "uniform" color vector (r,g,b,a)*/
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 myColor;\n"
"void main()\n"
"{\n"
"    FragColor = myColor;\n"
"}\0";



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

/*Utility midpoint function*/
float getMidPoint(float a, float b) { return (a + b) / 2.f; }

/*Recursive function to generate the data*/
void generateSierpinsky(float* vertices, int** indices, int left, int right, int top, int* indexIt, int level, int &vertexIt) {
	/*Stop if level reached*/
	if (level == TRIANGLE_LEVEL) return;
	
	/*Get Midpoints*/
	/*Left - Top*/
	vertices[vertexIt] = getMidPoint(vertices[left*3], vertices[top*3]); vertices[vertexIt + 1] = getMidPoint(vertices[left*3 + 1], vertices[top*3 + 1]); vertices[vertexIt + 2] = 0.f;
	vertexIt += 3;
	/*Top - Right*/
	vertices[vertexIt] = getMidPoint(vertices[top*3], vertices[right*3]); vertices[vertexIt + 1] = getMidPoint(vertices[top*3 + 1], vertices[right*3 + 1]); vertices[vertexIt + 2] = 0.f;
	vertexIt += 3;
	/*Left - Rigth*/
	vertices[vertexIt] = getMidPoint(vertices[left*3], vertices[right*3]); vertices[vertexIt + 1] = getMidPoint(vertices[left*3 + 1], vertices[right*3 + 1]); vertices[vertexIt + 2] = 0.f;
	vertexIt += 3;


	/*Auxiliar Variables (Needs Refactoring)*/
	int LT = (vertexIt - 9) / 3;
	int TR = (vertexIt - 6) / 3;
	int RL = (vertexIt - 3) / 3;

	/*Assign Indeces*/
	/*Left Bottom Triangle*/
	
	indices[level][indexIt[level]] = left; indices[level][indexIt[level]+1] = RL; indices[level][indexIt[level]+2] = LT;
	indexIt[level] += 3;
	/*Right Bottom Triangle*/
	
	indices[level][indexIt[level]] = RL; indices[level][indexIt[level] + 1] = right; indices[level][indexIt[level]+2] = TR;
	indexIt[level] += 3;
	/*Top Triangle*/
	indices[level][indexIt[level]] = LT; indices[level][indexIt[level] + 1] = TR; indices[level][indexIt[level]+2] = top;
	indexIt[level] += 3;

	

	/*Recursive Calls for other triangles*/
	/*L-B Triangle*/
	generateSierpinsky(vertices, indices, left, RL, LT, indexIt, level + 1, vertexIt);
	/*R-B Triangle*/
	generateSierpinsky(vertices, indices, RL, right, TR, indexIt, level + 1, vertexIt);
	/*Top Triangle*/
	generateSierpinsky(vertices, indices, LT, TR, top, indexIt, level + 1, vertexIt);
}
/*End of the recursive function*/



int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Sierpinsky Triangle", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, keycallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/*All the settings that we need in order to get running a single traingle*/
	/*Generate and compile vertex shader*/
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	/*Check if we succedeed compilating*/
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/*Generate and compile fragment shader*/
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	/*Check if ir was compiled*/
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	/*Create shader program*/
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	/*check if it was correctly linked*/
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}


	/*Delete shaders*/
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	/*Data for the triangle*/
	//Calculate total space needed
	int size = 3;
	int nn = 3;
	for (int i = 1; i < TRIANGLE_LEVEL; i++) {
		size += nn;
		nn *= 3;
	}
	size *= 3;
	
	float* vertices = new float[size]; //Array of vertices
	int vertexIt = 9;	   //Vertex Iterator

	/*First triangle*/
	vertices[0] = -1.0f; vertices[1] = -1.0f; vertices[2] = 0.0f; //Bottom Left
	vertices[3] = 1.0f; vertices[4] = -1.0f; vertices[5] = 0.0f;  //Bottom Right
	vertices[6] = 0.0f; vertices[7] = 1.0f; vertices[8] = 0.0f;   //Top


	int** indices;
	int indexIt[TRIANGLE_LEVEL];
	for (int i = 0; i < TRIANGLE_LEVEL; i++) indexIt[i] = 0;
	/*Allocate Spaces for indeces*/
	indices = new int* [TRIANGLE_LEVEL];
	int n = 3;
	for (int i = 0; i < TRIANGLE_LEVEL; i++) {
		indices[i] = new int[n];
		n *= 3;
	}
	
	/*First Triangle indexes*/
	for (int i = 0; i < 3; i++) { 
		indices[0][i] = i; 
		
	}

	/*Generate the data*/
	generateSierpinsky(vertices, indices, 0, 1, 2, indexIt, 1, vertexIt);
	

	unsigned int VAO[TRIANGLE_LEVEL], VBO[TRIANGLE_LEVEL], EBO[TRIANGLE_LEVEL];

	
	glGenVertexArrays(TRIANGLE_LEVEL, VAO);
	glGenBuffers(TRIANGLE_LEVEL, VBO);
	glGenBuffers(TRIANGLE_LEVEL, EBO);
	
	

	/*Bindigns for triangles*/
	int no = 3;
	for (int i = 0; i < TRIANGLE_LEVEL; i++) {
		glBindVertexArray(VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*size, vertices, GL_STATIC_DRAW);
		
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);		
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*no , indices[i], GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		no *= 3;
	}
	


	
	
	while (!glfwWindowShouldClose(window)) {

		/*Input*/
		processInput(window);

		/*Rendering comands*/
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		/*Draw*/
		glUseProgram(shaderProgram);
		int vertexColorLocation1 = glGetUniformLocation(shaderProgram, "myColor");
		
		
		for (int i = 0; i < until; i++) {			
			glUniform4f(vertexColorLocation1, colors[(3 * ((startColor + i) % 10))], colors[(3 * ((startColor + i) % 10)) + 1], colors[(3 * ((startColor + i) % 10)) + 2], 1.0f);
			glBindVertexArray(VAO[i]);
			glDrawElements(GL_TRIANGLES, nn, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		
		
		/*Swap Buffers*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	/*Deallocate Memory*/
	delete[] vertices;
	for (int i = 0; i < TRIANGLE_LEVEL; i++) delete[] indices[i];
	delete[] indices;
	glfwTerminate();
	return 0;

}