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
#include "Octree.hpp"
#include <deque>

namespace Game {
    struct OctreeSystem : AsyncECS::SystemChangedGameObject<const WorldBoundingBox>{
        
        OctreeSystem();
    
        void Update(AsyncECS::GameObject gameObject, const WorldBoundingBox& worldBoundingBox);
        
        void GameObjectRemoved(AsyncECS::GameObject gameObject);
        
        Octree<AsyncECS::GameObject> octree;
        
        std::deque<OctreeNode<AsyncECS::GameObject>> nodes;
        
        OctreeNode<AsyncECS::GameObject> defaultNode;
        
        void Query(const BoundingFrustum& frustum, std::vector<AsyncECS::GameObject>& gameObjects) const;
    };
}
