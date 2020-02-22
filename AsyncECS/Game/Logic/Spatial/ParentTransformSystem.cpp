//
//  TransformSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "ParentTransformSystem.hpp"

using namespace Game;

void ParentTransformSystem::Initialize() {

}

void ParentTransformSystem::Changed(const Parent& parent, DirtyTransform& dirtyTransform) {
    dirtyTransform.isDirty = true;
}

void ParentTransformSystem::Update(const Parent& parent, DirtyTransform& dirtyTransform) {
    dirtyTransform.isDirty = true;
}
