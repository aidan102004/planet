#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform vec3 uLightDir;
uniform mat3 rotateMatrix;
uniform mat3 lightMatrix;
uniform float alpha;
uniform sampler2D uTexture;

//perlin
uniform int octCount;
uniform int gridSize;
uniform int seed;
uniform float zComp;
uniform vec4 col1;
uniform vec4 col2;
uniform vec4 col3;
uniform vec4 col4; 
uniform vec4 thresholds;

float createPerlin(float x, float y);
float perlin(float x, float y, float z);
vec2 grad(int xPos, int yPos, float z);
float interpolate(float a0, float a1, float w);
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

    float px = sphereUV.x * 800; //gets the pixel value
    float py = sphereUV.y * 800;

    float val = createPerlin(px, py); //creates perlin value

    vec4 texColor; //map to color
    if (val <= thresholds.x) {
    texColor = col1 / 255.0;
    } else if (val <= thresholds.y) {
        texColor = col2 / 255.0;
    } else if (val <= thresholds.z) {
        texColor = col3 / 255.0;
    } else {
        texColor = col4 / 255.0;
    }
    float light = max(0.05, dot(normal, uLightDir * lightMatrix)); //rotate the light normals with the lightMatrix
    vec3 c = texColor.rgb * light; //sets the color
    c = mix(c, texColor.rgb, max(0.0, light - 0.5)); // brighten lit areas using texture color not white, linearly interpolates between c and texColor.rgb by the light value
    FragColor = vec4(c, circle * texColor.a * alpha); 
}

float createPerlin(float x, float y) {
    float freq = 1.0; //intensifies each octave
    float amp = 1.0; //controls how much each octave contributes to final val
    float val = 0.0;

    float z0 = floor(zComp); //gets the floored value of the time float
    float z1 = z0 + 1.0; //gets the next whole number
    float frac = zComp - z0; //finds the difference
    for(int i = 0; i < octCount; i++) {
        float sx = x * freq / gridSize; //scale the pixel coords into noise space
        float sy = y * freq / gridSize;
        float v0 = perlin(sx, sy, z0); //creates two perlin noise values for each time values
        float v1 = perlin(sx, sy, z1);
        val += mix(v0, v1, frac) * amp; //linearly interpolate between the two
        freq *= 2.0;
        amp *= 0.5; 
    }
    val = clamp(val, -1.0, 1.0); //clamp values between -1.0 and 1.0
    return val;
}

float perlin(float x, float y, float z) {
    int x0 = int(x); //finds corner points of the grid cell
    int y0 = int(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float fracx = x - float(x0); //finds the distance from the x and y 
    float fracy = y - float(y0);
    //get the dot product for each corner based on the distance vector to that corner and the gradient vector made with the hash function
    float c1 = dot(vec2(x-x0, y-y0), grad(x0, y0, z)); //pass in the z float which may be time
    float c2 = dot(vec2(x-x1, y-y0), grad(x1, y0, z));
    float c3 = dot(vec2(x-x0, y-y1), grad(x0, y1, z));
    float c4 = dot(vec2(x-x1, y-y1), grad(x1, y1, z));
    float ix0 = interpolate(c1, c2, fracx); //interpolate for the x value on top two corners
    float ix1 = interpolate(c3, c4, fracx); //on bottom two corners
    float val = interpolate(ix0, ix1, fracy); //interpolate between those values using the y weight
    return val;
}

vec2 grad(int xPos, int yPos, float z) {
    const uint w = 32u;
    const uint s = w / 2u;

    uint a = uint(xPos + seed);
    uint b = uint(yPos + seed);

    a *= 3284157443u;
    b ^= (a << s) | (a >> (w - s));
    b *= 1911520717u;

    a ^= (b << s) | (b >> (w - s));
    a *= 2048419325u;

    a ^= uint(z) * 1234567891u;

    float random = float(a) / float(0xffffffffu) * 6.28318530718;

    return vec2(sin(random), cos(random));
}
float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0; //smoothstep interpolation, blends between a and a1 based on w
}
