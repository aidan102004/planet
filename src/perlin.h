#ifndef PERLIN_H
#define PERLIN_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "pallette.h"

class Perlin{
    public:
        int gridSize;
        int seed;
        int octaveCount;
        int w;
        int h;
        glm::vec4 col1;
        glm::vec4 col2;
        glm::vec4 col3;
        glm::vec4 col4;
        Perlin(unsigned char* pixels, int octaveCount, int seed, int gridSize, std::pair<int, int> dim, Pallete pallete);
        void createPerlin(unsigned char* pixels);

        float createVal(float x, float y);
        glm::vec2 grad(int xPos, int yPos);
        float interpolate(float a0, float a1, float w);
};

#endif
