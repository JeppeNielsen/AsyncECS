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
#include "LocalTransformSystem.hpp"
#include "DirtyTransform.hpp"

namespace Game {
    struct ParentTransformSystem :
        AsyncECS::SystemChanged<const Parent, DirtyTransform>,
        AsyncECS::NoSystemDependencies,
        AsyncECS::NoComponentView {
        
        void Initialize();
        void Changed(const Parent& parent, DirtyTransform& dirtyTransform);
        void Update(const Parent& parent, DirtyTransform& dirtyTransform);
    
    };
}
