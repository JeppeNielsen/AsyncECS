//
//  AssignChildrenSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 23/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Hierarchy.hpp"
#include "SystemChangedGameObject.hpp"
#include "ComponentView.hpp"

namespace Game {
    struct HierarchySystem :
        AsyncECS::SystemChangedGameObject<Hierarchy>,
        AsyncECS::SceneModifier<Hierarchy>,
        AsyncECS::ComponentView<Hierarchy> {
        
        void Initialize();
        void Update(AsyncECS::GameObject gameObject, Hierarchy& hierarchy);
        void GameObjectRemoved(AsyncECS::GameObject gameObject);
    };
}
