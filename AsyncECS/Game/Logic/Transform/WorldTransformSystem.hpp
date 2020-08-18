//
//  TransformSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Scene.hpp"
#include "Hierarchy.hpp"
#include "LocalTransform.hpp"
#include "WorldTransform.hpp"
#include "HierarchyWorldTransformSystem.hpp"

namespace Game {
    struct WorldTransformSystem :
        AsyncECS::SystemChanged<const LocalTransform, const Hierarchy, WorldTransform>,
        AsyncECS::SystemDependencies<HierarchyWorldTransformSystem>,
        AsyncECS::ComponentView<const LocalTransform, const Hierarchy, WorldTransform> {
        
        void Initialize(HierarchyWorldTransformSystem&);
        void Changed(const LocalTransform& localTransform, const Hierarchy& hierarchy, WorldTransform& worldTransform);
        void Update(const LocalTransform& localTransform, const Hierarchy& hierarchy, WorldTransform& worldTransform);
    
        constexpr int EnableConcurrency() { return 4000; }
    };
}
