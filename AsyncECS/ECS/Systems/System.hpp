//
//  System.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "SystemBase.hpp"

namespace AsyncECS {

template<typename...T>
struct System : SystemBase<T...> {
    
    template<typename Components, typename ComponentObjects, typename SystemType>
    void Iterate(const Components& components, ComponentObjects& componentObjects) {
        const auto this_system = std::make_tuple((SystemType*)this);
        const auto& gameObjectsInSystem = this->template GetObjects<Components>(componentObjects).objects;
        for(const auto gameObject : gameObjectsInSystem) {
            const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
            const auto iterator = std::tuple_cat(this_system, componentValues);
            std::apply(&SystemType::Update, iterator);
        }
        
        for(const auto gameObject : gameObjectsInSystem) {
            this->template ChangeComponents(gameObject, components);
        }
    }
};

}
