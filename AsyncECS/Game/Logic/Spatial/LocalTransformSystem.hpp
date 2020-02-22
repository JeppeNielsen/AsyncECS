//
//  TransformSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Scene.hpp"
#include "LocalTransform.hpp"
#include "DirtyTransform.hpp"

namespace Game {
    struct LocalTransformSystem :
        AsyncECS::SystemChanged<const LocalTransform, DirtyTransform>,
        AsyncECS::NoDependencies,
        AsyncECS::NoComponentView {
        
        void Initialize();
        void Changed(const LocalTransform& localTransform, DirtyTransform& dirtyTransform);
        void Update(const LocalTransform& localTransform, DirtyTransform& dirtyTransform);
    
    };
}
