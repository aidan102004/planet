#include"vao.h"

VAO::VAO()
{
    glGenVertexArrays(1, &ID); //genereates the vao and stores its reference in ID
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
    VBO.Bind(); //binds opengls internal slot to this ID
    glVertexAttribPointer(layout, numComponents, type , GL_FALSE, stride, offset); //tells opengl how to interprate the raw data in the VBO
    glEnableVertexAttribArray(layout); //turns on attribute slot so vertex shader can read from it
    VBO.Unbind(); //unbinds it
}

void VAO::Bind() {
    glBindVertexArray(ID);
}

void VAO::Unbind() {
    glBindVertexArray(0);
}

void VAO::Delete() {
    glDeleteVertexArrays(1, &ID);
}