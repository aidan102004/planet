#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform vec3 color;
uniform vec3 uLightDir;
uniform mat3 rotateMatrix;
uniform mat3 lightMatrix;
uniform float alpha;
uniform sampler2D uTexture;

void main()
{
    float d = 1.0 - length(uv);
    float circle = step(0.0, d);
    float z = sqrt(max(0.0, 1.0 - dot(uv, uv)));
    vec3 normal = normalize(vec3(uv.x, uv.y, z));
    
    // rotate the normal by the matrix
    vec3 rotatedNormal = rotateMatrix * normal;

    // convert rotated 3D normal to spherical UV coords
    float longitude = atan(rotatedNormal.x, rotatedNormal.z);  // -PI to PI
    float latitude  = asin(rotatedNormal.y);                    // -PI/2 to PI/2

    // remap to 0-1
    vec2 sphereUV = vec2(
        longitude / (2.0 * 3.14159265) + 0.5,
        latitude  /        3.14159265  + 0.5
    );
    vec4 texColor = texture(uTexture, sphereUV);

    float light = max(0.0, dot(normal, (uLightDir * lightMatrix)));
    vec3 c = mix(vec3(0.0), color, light);
    c = mix(c, vec3(1.0), max(0.0, light - 0.5));

    FragColor = vec4(texture(uTexture, sphereUV).rgb * c, circle * texColor.a * alpha);
}