#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform vec3 color;
uniform vec3 uLightDir;
uniform mat3 rotateMatrix;
uniform float alpha;

void main()
{
    float d = 1.0 - length(uv);
    float circle = step(0.0, d);
    float z = sqrt(max(0.0, 1.0 - dot(uv, uv)));
    vec3 normal = normalize(vec3(uv.x, uv.y, z));
    float light = max(0.0, dot(normal, (uLightDir * rotateMatrix)));
    vec3 c = mix(vec3(0.0), color, light);
    c = mix(c, vec3(1.0), max(0.0, light - 0.5));
    FragColor = vec4(c, circle * alpha);
}