//
//  QuadTreeSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "ECS.hpp"
#include "WorldBoundingBox.hpp"
#include "QuadTree.hpp"

namespace Game {
    struct QuadTreeSystem : AsyncECS::SystemChangedGameObject<const WorldBoundingBox>{
        
        QuadTreeSystem();
    
        void Update(AsyncECS::GameObject gameObject, const WorldBoundingBox& worldBoundingBox);
        
        void GameObjectRemoved(AsyncECS::GameObject gameObject);
        
        Quadtree<AsyncECS::GameObject> quadTree;
        
        std::vector<QuadtreeNode<AsyncECS::GameObject>> nodes;
        
        QuadtreeNode<AsyncECS::GameObject> defaultNode;
    };
}
