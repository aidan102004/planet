#ifndef PLANET_CLASS_H
#define PLANET_CLASS_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "pallette.h"

struct Circle {
	glm::vec3 scale;
	glm::vec3 transform;
	float alpha;
	float z;
    int gridSize;
    int octCount;
    int seed;
    Pallete col;
};

#endif