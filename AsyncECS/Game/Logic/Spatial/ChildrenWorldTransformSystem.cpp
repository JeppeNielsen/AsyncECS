//
//  ChildrenWorldTransformSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 24/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "ChildrenWorldTransformSystem.hpp"

using namespace Game;

using namespace Game;

void ChildrenWorldTransformSystem::Initialize() {

}

void ChildrenWorldTransformSystem::Changed(const LocalTransform& localTransform, const Children& children, WorldTransform& worldTransform) {
    
}

void ChildrenWorldTransformSystem::Update(const LocalTransform& localTransform, const Children& children, WorldTransform& worldTransform) {
    
    worldTransform.isDirty = true;
    
    for(auto child : children.children) {
        GetComponents(child, [this](const LocalTransform& childLocalTransform, const Children& childChildren, WorldTransform& childWorldTransform) {
            Update(childLocalTransform, childChildren, childWorldTransform);
        });
    }
}
