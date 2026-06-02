#include"perlin.h"
#include"pallette.h"

Perlin::Perlin(unsigned char* pixels, int octaveCount, int seed, int gridSize, std::pair<int, int> dim, Pallete pallete) {
    this->octaveCount = octaveCount;
    this->seed = seed;
    this->gridSize = gridSize;
    this->w = dim.first;
    this->h = dim.second;
    this->col1 = pallete.col1;
    this->col2 = pallete.col2;
    this->col3 = pallete.col3;
    this->col4 = pallete.col4;
    createPerlin(pixels);
}

void Perlin::createPerlin(unsigned char* pixels)
{
   for (int x =0; x<w;x++) {
		for (int y = 0; y<h; y++) {
			int index = (y * w + x) * 4;

			float pixelVal = 0;
			float freq = 1;
			float amp = 1;

			for (int i = 0; i < octaveCount; i++) {
				pixelVal += createVal(x * freq / gridSize, y * freq / gridSize) * amp;
				freq *= 2.0f;
				amp  *= 0.5f;
			}
			if (pixelVal > 1.0) {
				pixelVal = 1.0;
			} else if (pixelVal < -1.0) {
				pixelVal = -1.0;
			}
			if (pixelVal >= -1.0f && pixelVal <= 0.0f) {
                pixels[index + 0] = (unsigned char)col1.r;
                pixels[index + 1] = (unsigned char)col1.g;
                pixels[index + 2] = (unsigned char)col1.b;
                pixels[index + 3] = (unsigned char)col1.a;
            } else if (pixelVal > 0.0f && pixelVal <= 0.1f) {
                pixels[index + 0] = (unsigned char)col2.r;
                pixels[index + 1] = (unsigned char)col2.g;
                pixels[index + 2] = (unsigned char)col2.b;
                pixels[index + 3] = (unsigned char)col2.a;
            } else if (pixelVal > 0.1f && pixelVal <= 0.14f) {
                pixels[index + 0] = (unsigned char)col3.r;
                pixels[index + 1] = (unsigned char)col3.g;
                pixels[index + 2] = (unsigned char)col3.b;
                pixels[index + 3] = (unsigned char)col3.a;
            } else {
                pixels[index + 0] = (unsigned char)col4.r;
                pixels[index + 1] = (unsigned char)col4.g;  
                pixels[index + 2] = (unsigned char)col4.b;   
                pixels[index + 3] = (unsigned char)col4.a;  
            }
                    }
	}
}

float Perlin::createVal(float x, float y) {
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float fracx = x - (float)x0;
	float fracy = y - (float)y0;

	float c1 = glm::dot(glm::vec2(x - x0, y - y0), grad(x0, y0));
	float c2 = glm::dot(glm::vec2(x - x1, y - y0), grad(x1, y0));
	float c3 = glm::dot(glm::vec2(x - x0, y - y1), grad(x0, y1));
	float c4 = glm::dot(glm::vec2(x - x1, y - y1), grad(x1, y1));
	float ix0 = interpolate(c1, c2, fracx);
	float ix1 = interpolate(c3, c4, fracx);
	float val = interpolate(ix0, ix1, fracy);
	return val;
}

glm::vec2 Perlin::grad(int xPos, int yPos) {
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

float Perlin::interpolate(float a0, float a1, float w) {
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}


