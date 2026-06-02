#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform vec3 uLightDir;
uniform mat3 rotateMatrix;
uniform mat3 lightMatrix;
uniform float alpha;
uniform sampler2D uTexture;

void main()
{
    float d = 1.0 - length(uv); //sets d to the distance from the center flipped so anything less than 0.0 is outside
    float circle = step(0.0, d); //step returns 0 if d less than 0 and 1 if d is greater than 0 for every rasterized pixel
    float z = sqrt(max(0.0, 1.0 - dot(uv, uv))); //use the spherical equation to solve for z normal
    vec3 normal = normalize(vec3(uv.x, uv.y, z)); //pass in the modified z component for the normals
    
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

    vec4 texColor = texture(uTexture, sphereUV); //looks up the pixel from the texture at this given coord

    float light = max(0.05, dot(normal, uLightDir * lightMatrix)); //rotate the light normals with the lightMatrix
    vec3 c = texColor.rgb * light; //sets the color
    c = mix(c, texColor.rgb, max(0.0, light - 0.5)); // brighten lit areas using texture color not white, linearly interpolates between c and texColor.rgb by the light value
    FragColor = vec4(c, circle * texColor.a * alpha); 
}