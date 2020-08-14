//
//  Vertex.h
//  Shaders
//
//  Created by Jeppe Nielsen on 23/06/15.
//  Copyright (c) 2015 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <stdio.h>
#include <glm/vec2.hpp>
#include "Color.hpp"
#include "OpenGL.hpp"
#include <map>

namespace Game {

struct VertexAttribute {
	VertexAttribute() {}
	VertexAttribute(GLint size, GLenum type, GLboolean normalized) :
		size(size), type(type), normalized(normalized) {}
    GLint size;
    GLenum type;
    GLboolean normalized;
    int offset;
    GLint glAttribute;
};

template<class T>
struct VertexAttributeType {
    static const VertexAttribute attribute;
};

template<typename T> const VertexAttribute VertexAttributeType<T>::attribute;

template<class Vertex>
class VertexDescription {
public:
    VertexDescription() {
        //id = Vertex::ID;
        stride = sizeof(Vertex);
        startOffset = (size_t)(&vertex);
        vertex.Initialize(*this);
    
        //for(auto& a : attributes) {
        //    std::cout<<"Name : " << a.first<< "  offset : " <<a.second.offset<<std::endl;
        //}
    }
    
public:
    const int Stride() { return stride; }
private:
    int stride;
    Vertex vertex;
    size_t startOffset;
    using AttributesList = std::map<std::string, VertexAttribute>;
    AttributesList attributes;
    int id;
public:

    const AttributesList& Attributes() { return attributes; }

    template<class T>
    void SetAttibute(T& member, std::string name) {
        VertexAttribute attribute = VertexAttributeType<T>::attribute;
        attribute.offset = (int)(((size_t)&member) - startOffset);
        attributes[name] = attribute;
    }
};

template<class T>
struct ShaderVariableType {
    static void SetValue(GLint location, const T& value);
    static GLenum GetType();
};

struct Vertex {
    glm::vec2 Position;
    glm::vec2 TextureCoords;
    Color Color;

    void Initialize(VertexDescription<Vertex>& description) {
        description.SetAttibute(Position, "Position");
        description.SetAttibute(TextureCoords, "TexCoords");
        description.SetAttibute(Color, "Color");
    }
};

}
