#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
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
#include"circle.h"


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

int gridSize = 180;
std::pair<Circle, Circle> randomizePlanet(std::mt19937& rng);
double gaussianRandom(std::mt19937& rng, double mean, double stdDev);
glm::vec4 randomNaturalColor(std::mt19937& rng);
void drawCircle(VAO& vao, Shader& shaderProgram, GLuint ldUniform, glm::vec3 ldr, GLuint rot, GLuint lrot, glm::mat3 rotMtx, glm::mat3 lightRot, glm::mat4 proj, glm::mat4 view, glm::mat4 model, Circle c);
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
	//circle 1
	VAO1.Bind();
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	int seed = 0, cloudSeed = 0, moonSeed = 0;
	srand(time(NULL));
	seed = rand();
	cloudSeed = rand();
	moonSeed = rand();

	Pallete earthPallete = {glm::vec4(15, 70, 160, 255), glm::vec4(24, 107, 202, 255), glm::vec4(194, 178, 128, 255), glm::vec4(44, 112, 49, 255), glm::vec4(0.0, 0.1, 0.14, 1.0)};
	Pallete cloudPallete = {glm::vec4(237, 237, 237, 237),glm::vec4(34, 139, 34, 0), glm::vec4(34, 139, 34, 0), glm::vec4(34, 139, 34, 0), glm::vec4(-0.1, 0.1, 0.14, 1.0)};
	Circle earth = {glm::vec3(1.0f), glm::vec3(0.0f), 1.0, 1.0, gridSize, 6, seed, earthPallete};
	Circle cloud = {glm::vec3(1.2f), glm::vec3(0.0f), 1.0, (float)glfwGetTime() * 0.3f,  80, 4, cloudSeed, cloudPallete};
	Pallete moonPallete = {glm::vec4(170, 170, 165, 255), glm::vec4(110, 110, 115, 255), glm::vec4(60, 60, 65, 255), glm::vec4(220, 215, 200, 255), glm::vec4(0.2, 0.7, 0.8, 1.0)};

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


	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	static std::mt19937 rng(std::random_device{}());
	std::pair<Circle, Circle> gen = std::pair<Circle, Circle>(std::make_pair(earth, cloud));
	while (!glfwWindowShouldClose(window))
	{
		glClearColor((float)6/255, (float)5/255, (float)33/255, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		static bool wasPressed = false;
		bool isPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
		if (isPressed && !wasPressed) {
			gen = randomizePlanet(rng);
		}
		wasPressed = isPressed;
        gen.second.z = (float)glfwGetTime() * 0.3f;
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

		glm::mat3 lightMatrix = glm::mat3(
			cos(-angle * 0.4f), 0, sin(-angle* 0.4f),
			0, 1, 0,
			-sin(-angle* 0.4f), 0, cos(-angle* 0.4f));
		drawCircle(VAO1, shaderProgram, ldUniform, lightur, rot_uniform, light_uniform, rotMatrix, lightMatrix, proj, view, glm::mat4(1.0f), gen.first);

		drawCircle(VAO1, shaderProgram, ldUniform, lightur, rot_uniform, light_uniform, rotMatrix2, lightMatrix, proj, view, glm::mat4(1.0f), gen.second);

		//drawCircle(VAO1, shaderProgram, ldUniform, lightur, rot_uniform, light_uniform, rotMatrix2, rotMatrix, 1.0f, proj, view, glm::mat4(1.0f), glm::vec3(0.2f), glm::vec3(4.5f, 1.5f, 1.5f), moonPallete, 1.0, 80, 4, moonSeed);

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

void drawCircle(VAO& vao, Shader& shaderProgram, GLuint ldUniform, glm::vec3 ldr, GLuint rot, GLuint lrot, glm::mat3 rotMtx, glm::mat3 lightRot, glm::mat4 proj, glm::mat4 view, glm::mat4 model, Circle c) {

		glUniform1f(glGetUniformLocation(shaderProgram.ID, "zComp"), c.z);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "seed"), c.seed);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "gridSize"), c.gridSize);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "octCount"), c.octCount);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "thresholds"), c.col.thresholds.x, c.col.thresholds.y, c.col.thresholds.z, c.col.thresholds.w);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "col1"), c.col.col1.r, c.col.col1.g, c.col.col1.b, c.col.col1.a);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "col2"), c.col.col2.r, c.col.col2.g, c.col.col2.b, c.col.col2.a);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "col3"), c.col.col3.r, c.col.col3.g, c.col.col3.b, c.col.col3.a);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "col4"), c.col.col4.r, c.col.col4.g, c.col.col4.b, c.col.col4.a);
		model = glm::scale(model, c.scale);
		model = glm::translate(model, c.transform);
		glm::mat4 MVP = proj * view * model;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "mvp"), 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "alpha"), c.alpha);
		glUniform3f(ldUniform, ldr.x, ldr.y, ldr.z);
		glUniformMatrix3fv(rot, 1, GL_FALSE, glm::value_ptr(rotMtx));
		glUniformMatrix3fv(lrot, 1, GL_FALSE, glm::value_ptr(lightRot));
		vao.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	
}

std::pair<Circle, Circle> randomizePlanet(std::mt19937& rng) {
	Pallete p1 = {randomNaturalColor(rng), randomNaturalColor(rng), randomNaturalColor(rng), randomNaturalColor(rng), glm::vec4(0.0, 0.1, 0.14, 1.0)};
	float s1 = std::clamp(gaussianRandom(rng, 1.0, 0.1), 0.2, 2.5);
	Circle c1 = {glm::vec3(s1), glm::vec3(0.0f), 1.0f, 1.0f, (int)std::clamp(gaussianRandom(rng, 180.0, 20.0), 100.0, 250.0), (int)std::clamp(gaussianRandom(rng, 6.0, 2.0), 2.0, 10.0), (int)rand(), p1 };
	Pallete p2 = {randomNaturalColor(rng), glm::vec4(34, 139, 34, 0), glm::vec4(34, 139, 34, 0), glm::vec4(34, 139, 34, 0), glm::vec4(-0.1, 0.1, 0.14, 1.0)};
	Circle c2 = {glm::vec3(s1 + 0.2f), glm::vec3(0.0f), 0.8f, (float)glfwGetTime() * 0.2f + (rand() % 1000) / 1000.0f * 0.4f, (int)std::clamp(gaussianRandom(rng, 80.0, 20.0), 40.0, 120.0), (int)std::clamp(gaussianRandom(rng, 4.0, 1.0), 2.0, 6.0), (int)rand(), p2 };
	return {c1, c2};
}

double gaussianRandom(std::mt19937& rng, double mean, double stdDev) {
    std::normal_distribution<double> dist(mean, stdDev);
    return dist(rng);
}

glm::vec4 randomNaturalColor(std::mt19937& rng) {
    std::normal_distribution<float> dist(128.0f, 50.0f);
    float r = std::clamp(dist(rng), 0.0f, 255.0f);
    float g = std::clamp(dist(rng), 0.0f, 255.0f);
    float b = std::clamp(dist(rng), 0.0f, 255.0f);
    return glm::vec4(r, g, b, 255.0f);
}