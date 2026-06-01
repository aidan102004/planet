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

const int width = 800, height = 800;

unsigned char pixels[width * height * 4];

unsigned char cloudpixels[width * height * 4];

int gridSize = 180;
void createPerlin(unsigned char* pixels, int octaveCount, int seed);
float perlin(float x, float y, int seed);
glm::vec2 grad(int xPos, int yPos, int seed);
float interpolate(float a0, float a1, float w);
void createCloudPerlin(unsigned char* pixels, int octaveCount, int seed);
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

	int seed = 0;
	int cloudSeed = 0;
	srand(time(NULL));
	seed = rand();
	cloudSeed = rand();
	createPerlin(pixels, 6, seed);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	createCloudPerlin(cloudpixels, 2, cloudSeed);
	GLuint cloudID;
	glGenTextures(1, &cloudID);
	glBindTexture(GL_TEXTURE_2D, cloudID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, cloudpixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//colors
	glm::vec3 blue = glm::vec3((float)24/255,(float)107/255,(float)202/255);
	glm::vec3 grey = glm::vec3((float)224/255,(float)224/255,(float)224/255);


	GLuint uniform = glGetUniformLocation(shaderProgram.ID, "color");
	glm::vec3 lightur = glm::normalize(glm::vec3(1.0,1.0,1.0));
	GLuint uniform1 = glGetUniformLocation(shaderProgram.ID, "uLightDir");
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
		GLint location = glGetUniformLocation(shaderProgram.ID, "uTexture");
		glUniform1i(location, 0);

		glActiveTexture(GL_TEXTURE0);              
		glBindTexture(GL_TEXTURE_2D, textureID);   
		double currTime = glfwGetTime();
		float deltaTime = (float)(currTime - lastTime);
		lastTime = currTime;
		angle += deltaTime * 0.5f;
		glm::mat3 rotMatrix = glm::mat3(
		cos(angle), 0, sin(angle),
		0, 1, 0,
		-sin(angle), 0, cos(angle));
		//finish drawing first circle 
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "scalar"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "alpha"), 1.0f);
		glUniform3f(uniform, blue.x, blue.y, blue.z);
		glUniform3f(uniform1, lightur.x, lightur.y, lightur.z);
		glUniformMatrix3fv(rot_uniform, 1, GL_FALSE, glm::value_ptr(rotMatrix));
		glUniformMatrix3fv(light_uniform, 1, GL_FALSE, glm::value_ptr(rotMatrix));
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, cloudID);    
		glUniform1i(location, 1);                 
		angle2 += deltaTime * 0.4f;
		glm::mat3 rotMatrix2 = glm::mat3(
		cos(angle2 + offset), 0, sin(angle2 + offset),
		0, 1, 0,
		-sin(angle2 + offset), 0, cos(angle2 + offset));
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "scalar"), 1.2f);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "alpha"), 0.5f);
		glUniform3f(uniform, grey.x, grey.y, grey.z);
		glUniform3f(uniform1, lightur.x, lightur.y, lightur.z);
		glUniformMatrix3fv(rot_uniform, 1, GL_FALSE, glm::value_ptr(rotMatrix2));
		glUniformMatrix3fv(light_uniform, 1, GL_FALSE, glm::value_ptr(rotMatrix));
		VAO2.Bind();
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

void createPerlin(unsigned char* pixels, int octaveCount, int seed) {

	for (int x =0; x<width;x++) {
		for (int y = 0; y<height; y++) {
			int index = (y * width + x) * 4;

			float pixelVal = 0;
			float freq = 1;
			float amp = 1;

			for (int i = 0; i < octaveCount; i++) {
				pixelVal += perlin(x * freq / gridSize, y * freq / gridSize, seed) * amp;
				freq *= 2.0f;
				amp  *= 0.5f;
				//pixelVal *= 1.2;
			}
			if (pixelVal > 1.0) {
				pixelVal = 1.0;
			} else if (pixelVal < -1.0) {
				pixelVal = -1.0;
			}
			if (pixelVal >= -1.0f && pixelVal <= 0.0f) {
				pixels[index + 0] = 24;   
				pixels[index + 1] = 107;  
				pixels[index + 2] = 202;  
				pixels[index + 3] = 255;  
			} else {
				pixels[index + 0] = 34;  
				pixels[index + 1] = 139;  
				pixels[index + 2] = 34;   
				pixels[index + 3] = 255;  
			}
		}
	}
}

void createCloudPerlin(unsigned char* pixels, int octaveCount, int seed) {

	for (int x =0; x<width;x++) {
		for (int y = 0; y<height; y++) {
			int index = (y * width + x) * 4;

			float pixelVal = 0;
			float freq = 1;
			float amp = 1;

			for (int i = 0; i < octaveCount; i++) {
				pixelVal += perlin(x * freq / 80, y * freq / 80, seed) * amp;
				freq *= 2.0f;
				amp  *= 0.5f;
				//pixelVal *= 1.2;
			}
			if (pixelVal > 1.0) {
				pixelVal = 1.0;
			} else if (pixelVal < -1.0) {
				pixelVal = -1.0;
			}
			if (pixelVal >= -1.0f && pixelVal <= 0.0f) {
				pixels[index + 0] = 213;   
				pixels[index + 1] = 213;  
				pixels[index + 2] = 213;  
				pixels[index + 3] = 213;  
			} else {
				pixels[index + 0] = 34;  
				pixels[index + 1] = 139;  
				pixels[index + 2] = 34;   
				pixels[index + 3] = 0;  
			}
		}
	}
}

float perlin(float x, float y, int seed) {
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float fracx = x - (float)x0;
	float fracy = y - (float)y0;

	float c1 = glm::dot(glm::vec2(x - x0, y - y0), grad(x0, y0, seed));
	float c2 = glm::dot(glm::vec2(x - x1, y - y0), grad(x1, y0, seed));
	float c3 = glm::dot(glm::vec2(x - x0, y - y1), grad(x0, y1, seed));
	float c4 = glm::dot(glm::vec2(x - x1, y - y1), grad(x1, y1, seed));
	float ix0 = interpolate(c1, c2, fracx);
	float ix1 = interpolate(c3, c4, fracx);
	float val = interpolate(ix0, ix1, fracy);
	return val;
}

glm::vec2 grad(int xPos, int yPos,int seed) {
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = xPos + seed, b = yPos + seed;
    a *= 3284157443;
 
    b ^= a << s | a >> w - s;
    b *= 1911520717;
 
    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    
    glm::vec2 v;
    v.x = sin(random);
    v.y = cos(random);
 
    return v;
}

float interpolate(float a0, float a1, float w) {
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

