//
//  QuadTreeSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "QuadTreeSystem.hpp"

using namespace Game;
using namespace AsyncECS;

QuadTreeSystem::QuadTreeSystem() {
    quadTree.SetBoundingBox({{0,0}, {1000,1000}});
    defaultNode.node = nullptr;
}

void QuadTreeSystem::Update(GameObject gameObject, const Game::WorldBoundingBox& worldBoundingBox) {

    if (gameObject >= nodes.size()) {
        nodes.resize(gameObject + 1, defaultNode);
    }
    
    auto& node = nodes[gameObject];
    node.data = gameObject;
    
    node.box = worldBoundingBox.bounds;
    
    if (!node.node) {
        quadTree.Insert(node);
    } else {
        quadTree.Move(node);
    }
}

void QuadTreeSystem::GameObjectRemoved(AsyncECS::GameObject gameObject) {
    
    if (gameObject >= nodes.size()) {
        return;
    }
    
    auto& node = nodes[gameObject];
    quadTree.Remove(node);
}

void QuadTreeSystem::Query(const BoundingBox &box, std::vector<AsyncECS::GameObject> &gameObjects) const {
    quadTree.Get(box, gameObjects);
}
