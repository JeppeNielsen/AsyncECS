//
//  TransformSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "WorldTransformSystem.hpp"

using namespace Game;

void WorldTransformSystem::Initialize(HierarchyWorldTransformSystem&) {

}

void WorldTransformSystem::Changed(const LocalTransform& localTransform, const Hierarchy& hierarchy, WorldTransform& worldTransform) {
    worldTransform.isDirty = true;
}

void WorldTransformSystem::Update(const LocalTransform& localTransform, const Hierarchy& hierarchy, WorldTransform& worldTransform) {
    if (!worldTransform.isDirty) {
        return;
    }
    worldTransform.isDirty = false;
    
    Matrix3x3 local;
    local.TRS(localTransform.position, localTransform.rotation, localTransform.scale);
    
    if (hierarchy.parent != AsyncECS::GameObjectNull) {
        WorldTransform* parentTransform;
        GetComponents(hierarchy.parent, [this, &parentTransform](const LocalTransform& parentLocalTransform, const Hierarchy& parentHierarchy, WorldTransform& parentWorldTransform) {
            Update(parentLocalTransform, parentHierarchy, parentWorldTransform);
            parentTransform = &parentWorldTransform;
        });
       
        worldTransform.world =local *
        parentTransform->world;
    } else {
        worldTransform.world = local;
    }
    worldTransform.worldInverse = worldTransform.world.Invert();
}
