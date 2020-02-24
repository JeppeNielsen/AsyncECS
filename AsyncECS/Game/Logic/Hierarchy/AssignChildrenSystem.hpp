//
//  AssignChildrenSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 23/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Parent.hpp"
#include "Children.hpp"
#include "SystemChangedGameObject.hpp"
#include "ComponentView.hpp"

namespace Game {
    struct AssignChildrenSystem :
        AsyncECS::SystemChangedGameObject<const Parent, Children>,
        AsyncECS::NoSystemDependencies,
        AsyncECS::ComponentView<Children> {
        
        void Initialize();
        void Update(GameObject gameObject, const Parent& parent, Children& children);
    
    };
}
