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
    template<typename ...T>
    struct OctreeSystem : AsyncECS::SystemChangedGameObject<const WorldBoundingBox, T...>{
        
        OctreeSystem() {
            octree.SetBoundingBox({{0,0,0}, {100000,100000,100000}});
            defaultNode.node = nullptr;
        }
    
        void Update(AsyncECS::GameObject gameObject, const WorldBoundingBox& worldBoundingBox, T& ... rest) {
            if (gameObject >= nodes.size()) {
                nodes.resize(gameObject + 1, defaultNode);
            }
            
            auto& node = nodes[gameObject];
            node.data = gameObject;
            node.box = worldBoundingBox.bounds;
            
            if (!node.node) {
                octree.Insert(node);
            } else {
                octree.Move(node);
            }
        }
        
        void GameObjectRemoved(AsyncECS::GameObject gameObject) {
            if (gameObject >= nodes.size()) {
                return;
            }
            
            auto& node = nodes[gameObject];
            octree.Remove(node);
        }
        
        
        void Query(const BoundingFrustum& frustum, std::vector<AsyncECS::GameObject> &gameObjects) const {
            octree.Get(frustum, gameObjects);
        }
        
        void Query(const Ray& ray, std::vector<AsyncECS::GameObject> &gameObjects) const {
            octree.Get(ray, gameObjects);
            //octree.GetRecursive(gameObjects);
        }

        Octree<AsyncECS::GameObject> octree;
        std::deque<OctreeNode<AsyncECS::GameObject>> nodes;
        OctreeNode<AsyncECS::GameObject> defaultNode;
   };
}
