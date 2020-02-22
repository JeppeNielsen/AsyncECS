//
//  TransformSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "WorldTransformSystem.hpp"

using namespace Game;

void WorldTransformSystem::Initialize() {

}

void WorldTransformSystem::Changed(const LocalTransform& localTransform, const Parent& parent, WorldTransform& worldTransform) {
    worldTransform.isDirty = true;
}

void WorldTransformSystem::Update(const LocalTransform& localTransform, const Parent& parent, WorldTransform& worldTransform) {
    if (!worldTransform.isDirty) {
        return;
    }
    worldTransform.isDirty = false;
    if (parent.parent != GameObjectNull) {
        WorldTransform* parentTransform;
        GetComponents(parent.parent, [this, &parentTransform](const LocalTransform& parentLocalTransform, const Parent& parentParent, WorldTransform& parentWorldTransform) {
            Update(parentLocalTransform, parentParent, parentWorldTransform);
            parentTransform = &parentWorldTransform;
        });
        worldTransform.world =
        parentTransform->world *
        Matrix3x3::CreateTranslation(localTransform.position) *
        Matrix3x3::CreateRotation(localTransform.rotation) *
        Matrix3x3::CreateScale(localTransform.scale);
    } else {
        worldTransform.world =
        Matrix3x3::CreateTranslation(localTransform.position) *
        Matrix3x3::CreateRotation(localTransform.rotation) *
        Matrix3x3::CreateScale(localTransform.scale);
    }
    worldTransform.worldInverse = worldTransform.world.Invert();
}
