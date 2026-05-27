#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
out vec2 uv;

uniform float scalar;

void main()
{
    gl_Position = vec4(aPos * scalar, 1.0);
    uv = aUV;
}