//
//  ChildrenWorldTransformSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 24/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Scene.hpp"
#include "Children.hpp"
#include "LocalTransform.hpp"
#include "WorldTransform.hpp"

namespace Game {
    struct ChildrenWorldTransformSystem :
        AsyncECS::SystemChanged<const LocalTransform, const Children, WorldTransform>,
        AsyncECS::NoSystemDependencies,
        AsyncECS::ComponentView<const LocalTransform, const Children, WorldTransform> {
        
        void Initialize();
        void Changed(const LocalTransform& localTransform, const Children& children, WorldTransform& worldTransform);
        void Update(const LocalTransform& localTransform, const Children& children, WorldTransform& worldTransform);
    
    };
}
