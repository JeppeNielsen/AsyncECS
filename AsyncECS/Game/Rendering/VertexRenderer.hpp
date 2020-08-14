//
//  VertexRenderer.h
//  Shaders
//
//  Created by Jeppe Nielsen on 23/06/15.
//  Copyright (c) 2015 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "OpenGL.hpp"

namespace Game {

template<class Vertex>
class VertexRenderer {
public:
    const static unsigned int MAX_VERTICES = 1<<16;
    const static unsigned int MAX_TRIANGLES = 1<<16;

    VertexRenderer() {
        ASSERT_GL(glGenBuffers(1, &vertexBuffer));
        ASSERT_GL(glGenBuffers(1, &indexBuffer));
        
        ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
        ASSERT_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_VERTICES, 0, GL_DYNAMIC_DRAW));
        
        ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer));
        ASSERT_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * MAX_TRIANGLES, 0, GL_DYNAMIC_DRAW));
    }

    ~VertexRenderer() {
        ASSERT_GL(glDeleteBuffers(1, &vertexBuffer));
        ASSERT_GL(glDeleteBuffers(1, &indexBuffer));
    }
    
    using Vertices = std::vector<Vertex>;
    using Triangles = std::vector<GLshort>;

public:
    
    void RenderVertices(const Vertices& vertices, const size_t verticesIndex, const size_t verticesCount,
                        const Triangles& triangles, const size_t trianglesIndex, const size_t trianglesCount
                        ) {
        
        ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
        ASSERT_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticesCount, &vertices[verticesIndex], GL_DYNAMIC_DRAW));
        
        ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer));
        ASSERT_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * trianglesCount, &triangles[trianglesIndex], GL_DYNAMIC_DRAW));
    
        ASSERT_GL(glDrawElements(GL_TRIANGLES, (int)trianglesCount, GL_UNSIGNED_SHORT, 0));
    }
   
private:
    GLuint vertexBuffer;
    GLuint indexBuffer;
};

}
