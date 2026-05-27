#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"texture.h"
#include"shaderClass.h"
#include"vao.h"
#include"vbo.h"
#include"ebo.h"


GLfloat vertices[] =
{
    // pos                // uv
    -0.5f, -0.5f, 0.0f,  -1.0f, -1.0f,
    -0.5f,  0.5f, 0.0f,  -1.0f,  1.0f,
     0.5f,  0.5f, 0.0f,   1.0f,  1.0f,
     0.5f, -0.5f, 0.0f,   1.0f, -1.0f,
};

GLuint indices[] =
{
    0, 1, 2, // Upper triangle
    0, 2, 3  // Lower triangle
};

int width, height = 800;

float perlin(float x, float y);
float dotGridGradient(int ix, int iy, float x, float y);
glm::vec2 randomGradient(int ix, int iy);
float interpolate(float a0, float a1, float w);

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Planet", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, 800, 800);

	Shader shaderProgram("Resources/default.vert", "Resources/default.frag");



	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	GLuint uniform = glGetUniformLocation(shaderProgram.ID, "color");
	glm::vec3 lightur = glm::normalize(glm::vec3(1.0,1.0,1.0));
	GLuint uniform1 = glGetUniformLocation(shaderProgram.ID, "uLightDir");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram.Activate();
		glUniform3f(uniform, 0.0f, 0.8f, 0.4f);
		glUniform3f(uniform1, lightur.x, lightur.y, lightur.z);
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

float perlin(float x, float y) {
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float sx = x - (float)x0;
	float sy = y - (float)y0;

	float n0 = dotGridGradient(x0, y0, x, y);
	float n1 = dotGridGradient(x1, y0, x, y);
	float ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	float ix1 = interpolate(n0, n1, sx);

	float value = interpolate(ix0, ix1, sy);

	return value;
}

float dotGridGradient(int ix, int iy, float x, float y) {
	glm::vec2 gradient = randomGradient(ix, iy);

	float dx = x - (float)ix;
	float dy = y - (float)iy;
	return (dx * gradient.x + dy * gradient.y);
}

glm::vec2 randomGradient(int ix, int iy) {
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w/2;
	unsigned a = ix, b = iy;
	a *= 3284157443;

	b ^= a << s | a >> w - s;
	b *= 1911520717;

	a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265) / ~(~0u >> 1);

	glm::vec2 v;
	v.x = sin(random);
	v.y = cos(random);
	return v;

}

float interpolate(float a0, float a1, float w) {
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}