//
//  Vertex.cpp
//  Shaders
//
//  Created by Jeppe Nielsen on 03/07/15.
//  Copyright (c) 2015 Jeppe Nielsen. All rights reserved.
//

#include "Vertex.hpp"
#include "Math.hpp"

using namespace Game;
using namespace glm;

template<> const VertexAttribute VertexAttributeType<vec3>::attribute = VertexAttribute(  3, GL_FLOAT, GL_FALSE );
template<> const VertexAttribute VertexAttributeType<vec2>::attribute = VertexAttribute(2, GL_FLOAT, GL_FALSE);
template<> const VertexAttribute VertexAttributeType<Color>::attribute = VertexAttribute(4, GL_UNSIGNED_BYTE, GL_TRUE);

template<> void ShaderVariableType<float>::SetValue(GLint location, const float &value) {
    glUniform1f(location, value);
}
template<> GLenum ShaderVariableType<float>::GetType() { return GL_FLOAT; }

template<> void ShaderVariableType<vec2>::SetValue(GLint location, const vec2 &value) {
    glUniform2f(location, value.x, value.y);
}
template<> GLenum ShaderVariableType<vec2>::GetType() { return GL_FLOAT_VEC2; }

template<> void ShaderVariableType<vec3>::SetValue(GLint location, const vec3 &value) {
    glUniform3f(location, value.x, value.y, value.z);
}
template<> GLenum ShaderVariableType<vec3>::GetType() { return GL_FLOAT_VEC3; }

template<> void ShaderVariableType<mat4x4>::SetValue(GLint location, const mat4x4 &value) {
    //glUniformMatrix4fv(location, 1, GL_FALSE, value.GetGlMatrix());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
template<> GLenum ShaderVariableType<mat4x4>::GetType() { return GL_FLOAT_MAT4; }

template<> void ShaderVariableType<Color>::SetValue(GLint location, const Color &value) {
    glUniform4f(location, value.R(), value.G(), value.B(), value.A());
}
template<> GLenum ShaderVariableType<Color>::GetType() { return GL_FLOAT_VEC4; }
