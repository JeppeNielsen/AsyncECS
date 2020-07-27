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
#include <deque>

namespace Game {
    struct QuadTreeSystem : AsyncECS::SystemChangedGameObject<const WorldBoundingBox>{
        
        QuadTreeSystem();
    
        void Update(AsyncECS::GameObject gameObject, const WorldBoundingBox& worldBoundingBox);
        
        void GameObjectRemoved(AsyncECS::GameObject gameObject);
        
        Quadtree<AsyncECS::GameObject> quadTree;
        
        std::deque<QuadtreeNode<AsyncECS::GameObject>> nodes;
        
        QuadtreeNode<AsyncECS::GameObject> defaultNode;
        
        void Query(const BoundingBox& box, std::vector<AsyncECS::GameObject>& gameObjects) const;
    };
}
