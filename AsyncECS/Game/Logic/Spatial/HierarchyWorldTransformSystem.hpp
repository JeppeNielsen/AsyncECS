//
//  ChildrenWorldTransformSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 24/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Scene.hpp"
#include "Hierarchy.hpp"
#include "LocalTransform.hpp"
#include "WorldTransform.hpp"

namespace Game {
    struct HierarchyWorldTransformSystem :
        AsyncECS::SystemChanged<const LocalTransform, const Hierarchy, WorldTransform>,
        AsyncECS::ComponentView<const LocalTransform, const Hierarchy, WorldTransform> {
        
        void Initialize();
        void Changed(const LocalTransform& localTransform, const Hierarchy& hierarchy, WorldTransform& worldTransform);
        void Update(const LocalTransform& localTransform, const Hierarchy& hierarchy, WorldTransform& worldTransform);
    
    };
}
