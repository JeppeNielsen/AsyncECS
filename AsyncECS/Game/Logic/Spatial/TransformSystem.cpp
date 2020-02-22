//
//  TransformSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "TransformSystem.hpp"

using namespace Game;

void TransformSystem::Initialize() {

}

void TransformSystem::Changed(const LocalTransform& localTransform, const Parent& parent,const DirtyTransform& dirtyTransform, WorldTransform& worldTransform) {

}

void TransformSystem::Update(const LocalTransform& localTransform, const Parent& parent,const DirtyTransform& dirtyTransform, WorldTransform& worldTransform) {
    if (!dirtyTransform.isDirty) {
        return;
    }
    //dirtyTransform.isDirty = false;

    if (parent.parent != GameObjectNull) {
        WorldTransform* parentTransform;
        GetComponents(parent.parent, [this, &parentTransform](const LocalTransform& parentLocalTransform, const Parent& parentParent, const DirtyTransform& parentDirtyTransform, WorldTransform& parentWorldTransform) {
            if (parentDirtyTransform.isDirty) {
                Update(parentLocalTransform, parentParent, parentDirtyTransform, parentWorldTransform);
                parentTransform = &parentWorldTransform;
            }
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
