//
//  QuadTreeNode.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "BoundingBox.hpp"

namespace Game {
    template<typename T>
    struct Quadtree;

    template<typename T>
    struct QuadtreeNode {
        BoundingBox box;
        Quadtree<T>* node;
        size_t childIndex;
        T data;
    };
}
