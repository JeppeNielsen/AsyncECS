//
//  QuadTreeSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "OctreeSystem.hpp"

using namespace Game;
using namespace AsyncECS;

OctreeSystem::OctreeSystem() {
    octree.SetBoundingBox({{0,0,0}, {100000,100000,100000}});
    defaultNode.node = nullptr;
}

void OctreeSystem::Update(GameObject gameObject, const Game::WorldBoundingBox& worldBoundingBox) {

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

void OctreeSystem::GameObjectRemoved(AsyncECS::GameObject gameObject) {
    
    if (gameObject >= nodes.size()) {
        return;
    }
    
    auto& node = nodes[gameObject];
    octree.Remove(node);
}

void OctreeSystem::Query(const BoundingFrustum& frustum, std::vector<AsyncECS::GameObject> &gameObjects) const {
    octree.Get(frustum, gameObjects);
    //octree.GetRecursive(gameObjects);
}
