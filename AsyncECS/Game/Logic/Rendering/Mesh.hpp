//
//  Mesh.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 02/08/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Vertex.hpp"
#include <vector>

namespace Game {
    struct Mesh {
        using Vertices = std::vector<Vertex>;
        using Triangles = std::vector<GLshort>;
        
        Vertices vertices;
        Triangles triangles;
    };
}
