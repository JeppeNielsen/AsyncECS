//
//  TransformSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "WorldTransformSystem.hpp"
#include <glm/mat3x3.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace Game;
using namespace glm;



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
    
    mat3x3 local;
    float Sin = sinf(localTransform.rotation) * localTransform.scale.x;
    float Cos = cosf(localTransform.rotation) * localTransform.scale.y;
    local[0][0] = Cos; local[0][1] = Sin; local[0][2] = 0;
    local[1][0] = -Sin; local[1][1] = Cos; local[1][2] = 0;
    local[2][0] = localTransform.position.x; local[2][1] = localTransform.position.y; local[2][2] = 1;

    if (hierarchy.parent != AsyncECS::GameObjectNull) {
        WorldTransform* parentTransform;
        GetComponents(hierarchy.parent, [this, &parentTransform](const LocalTransform& parentLocalTransform, const Hierarchy& parentHierarchy, WorldTransform& parentWorldTransform) {
            Update(parentLocalTransform, parentHierarchy, parentWorldTransform);
            parentTransform = &parentWorldTransform;
        });
        worldTransform.world = parentTransform->world * local;
    } else {
        worldTransform.world = local;
    }
    worldTransform.worldInverse = glm::inverse(worldTransform.world); //worldTransform.world.Invert();
}
