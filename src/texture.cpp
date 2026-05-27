#include"texture.h"

Texture::Texture(const char* image, GLenum texType, GLuint slot, GLenum format, GLenum pixelType)
{
    //assigns the type of texture to the texture object
    type = texType;
    int widthImg, heightImg, numColCh; //stores width height and color channels of the image
    stbi_set_flip_vertically_on_load(true); //vertically flips it because stb reads it upside down
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0); //loads the image and creates a pointer to the image data

    glGenTextures(1, &ID); //generates an opengl texture object and writes it to ID

    glActiveTexture(GL_TEXTURE0 + slot); //assigns texture to a texture unit
    unit = slot;
    glBindTexture(texType, ID);

    //configures the type of algorithm used to minimise and magnify the textuyre
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //configure the way the texture repeats
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //assigns the image to the opengl texture object
    glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
    glGenerateMipmap(texType); //generates mp maps

    stbi_image_free(bytes); //delete the image data
    glBindTexture(texType, 0); //unbinds 
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
    GLuint texUni = glGetUniformLocation(shader.ID, uniform); //gets location of the unfiform
    shader.Activate(); //activates the program
    glUniform1f(texUni, unit); //sets the value of the uniform
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit); //allows us to use multiple textures on the same object
    glBindTexture(type, ID);
}

void Texture::Unbind()
{
    glBindTexture(type, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}