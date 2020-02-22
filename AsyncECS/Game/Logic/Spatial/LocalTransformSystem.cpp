//
//  TransformSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "LocalTransformSystem.hpp"

using namespace Game;

void LocalTransformSystem::Initialize() {

}


void LocalTransformSystem::Changed(const LocalTransform& localTransform, DirtyTransform& dirtyTransform) {
    dirtyTransform.isDirty = true;
}

void LocalTransformSystem::Update(const LocalTransform& localTransform, DirtyTransform& dirtyTransform) {
    dirtyTransform.isDirty = true;
}
