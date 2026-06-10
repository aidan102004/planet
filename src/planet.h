#ifndef PLANET_CLASS_H
#define PLANET_CLASS_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Planet {
	glm::vec4 col1;
	glm::vec4 col2;
	glm::vec4 col3;
	glm::vec4 col4;
	glm::vec4 thresholds;
};

#endif