//
//  TransformSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Scene.hpp"
#include "Parent.hpp"
#include "LocalTransform.hpp"
#include "WorldTransform.hpp"

namespace Game {
    struct WorldTransformSystem :
        AsyncECS::SystemChanged<const LocalTransform, const Parent, WorldTransform>,
        AsyncECS::NoDependencies,
        AsyncECS::ComponentView<const LocalTransform, const Parent, WorldTransform> {
        
        void Initialize();
        void Changed(const LocalTransform& localTransform, const Parent& parent, WorldTransform& worldTransform);
        void Update(const LocalTransform& localTransform, const Parent& parent, WorldTransform& worldTransform);
    
    };
}
