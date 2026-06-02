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
#include"perlin.h"
#include"pallette.h"


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

const int width = 800, height = 800;

unsigned char pixels[width * height * 4];

unsigned char cloudpixels[width * height * 4];

int gridSize = 180;
void drawCircle(VAO& vao, Shader& shaderProgram, GLuint ldUniform, glm::vec3 ldr, GLuint rot, GLuint lrot, GLuint tex, int texLoc, GLenum texSlot, glm::mat3 rotMtx, glm::mat3 lightRot, float scale, float alpha);
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Planet", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

	Shader shaderProgram("Resources/default.vert", "Resources/default.frag");


	//circle 1s vao, vbo, ebo
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));
	//circle 2s vao, vbo, ebo
	VAO VAO2;
	VAO2.Bind();
	VBO VBO2(vertices, sizeof(vertices));
	EBO EBO2(indices, sizeof(indices));
	VAO2.Unbind();

	//circle 1
	VAO1.Bind();
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	VAO2.Bind();
	VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
    VAO2.LinkAttrib(VBO2, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO2.Unbind();
	VBO2.Unbind();
	EBO2.Unbind();

	int seed = 0, cloudSeed = 0;
	srand(time(NULL));
	seed = rand();
	cloudSeed = rand();

	Pallete earthPallete = {glm::vec4(15, 70, 160, 255), glm::vec4(24, 107, 202, 255), glm::vec4(194, 178, 128, 255), glm::vec4(44, 112, 49, 255) };
	Perlin perlin(pixels, 6, seed, gridSize, {width, height}, earthPallete);
	
	GLuint perlinTex;
	glGenTextures(1, &perlinTex);
	glBindTexture(GL_TEXTURE_2D, perlinTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	Pallete cloudPallete = {glm::vec4(237, 237, 237, 237),glm::vec4(34, 139, 34, 0), glm::vec4(34, 139, 34, 0), glm::vec4(34, 139, 34, 0)};
	Perlin cloudPerlin(cloudpixels, 4, cloudSeed, 80, {width, height}, cloudPallete);
	GLuint cloudTex;
	glGenTextures(1, &cloudTex);
	glBindTexture(GL_TEXTURE_2D, cloudTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, cloudpixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glm::vec3 lightur = glm::normalize(glm::vec3(1.0,1.0,1.0));
	GLuint ldUniform = glGetUniformLocation(shaderProgram.ID, "uLightDir");
	float angle = 0.0f;
	float angle2 = 0.0f;
	
	GLuint rot_uniform = glGetUniformLocation(shaderProgram.ID, "rotateMatrix");
	GLuint light_uniform = glGetUniformLocation(shaderProgram.ID, "lightMatrix");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	double lastTime = glfwGetTime();
	float offset = glm::radians(50.0f);	

	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor((float)6/255, (float)5/255, (float)33/255, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Activate();
		
		double currTime = glfwGetTime();
		float deltaTime = (float)(currTime - lastTime);
		lastTime = currTime;
		angle += deltaTime * 0.2f;
		angle2 += deltaTime * 0.3f; // cloud rotates faster

		glm::mat3 rotMatrix = glm::mat3(
			cos(angle), 0, sin(angle),
			0, 1, 0,
			-sin(angle), 0, cos(angle));

		glm::mat3 rotMatrix2 = glm::mat3(
			cos(angle2 + offset), 0, sin(angle2 + offset),
			0, 1, 0,
			-sin(angle2 + offset), 0, cos(angle2 + offset));
		drawCircle(VAO1, shaderProgram, ldUniform, lightur, rot_uniform, light_uniform, perlinTex, 0, GL_TEXTURE0, rotMatrix, rotMatrix, 1.0f, 1.0f);


		drawCircle(VAO2, shaderProgram, ldUniform, lightur, rot_uniform, light_uniform, cloudTex, 1, GL_TEXTURE1, rotMatrix2, rotMatrix, 1.2f, 0.8f);

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

void drawCircle(VAO& vao, Shader& shaderProgram, GLuint ldUniform, glm::vec3 ldr, GLuint rot, GLuint lrot, GLuint tex, int texLoc, GLenum texSlot, glm::mat3 rotMtx, glm::mat3 lightRot, float scale, float alpha) {
		GLint location = glGetUniformLocation(shaderProgram.ID, "uTexture");
		glUniform1i(location, texLoc);
		glActiveTexture(texSlot);              
		glBindTexture(GL_TEXTURE_2D, tex);  
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "scalar"), scale);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "alpha"), alpha);
		glUniform3f(ldUniform, ldr.x, ldr.y, ldr.z);
		glUniformMatrix3fv(rot, 1, GL_FALSE, glm::value_ptr(rotMtx));
		glUniformMatrix3fv(lrot, 1, GL_FALSE, glm::value_ptr(lightRot));
		vao.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	
}
