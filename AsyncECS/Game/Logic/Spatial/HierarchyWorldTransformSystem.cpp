//
//  ChildrenWorldTransformSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 24/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "HierarchyWorldTransformSystem.hpp"

using namespace Game;

void HierarchyWorldTransformSystem::Changed(const LocalTransform& localTransform, const Hierarchy& hierarchy, WorldTransform& worldTransform) {
    
}

void HierarchyWorldTransformSystem::Update(const LocalTransform& localTransform, const Hierarchy& hierarchy, WorldTransform& worldTransform) {
    worldTransform.isDirty = true;
    
    for(auto child : hierarchy.children) {
        GetComponents(child, [this](const LocalTransform& childLocalTransform, const Hierarchy& childHierarchy, WorldTransform& childWorldTransform) {
            Update(childLocalTransform, childHierarchy, childWorldTransform);
        });
    }
}
