//
//  System.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "SystemBase.hpp"
#include "TaskRunner.hpp"

namespace AsyncECS {

template<typename...T>
struct System : SystemBase<T...> {
    
    TaskRunner taskRunner;
    
    template<typename Components, typename SystemType>
    void Iterate(const Components& components) {
        
        const auto this_system = std::make_tuple((SystemType*)this);
        const auto& gameObjectsInSystem = this->objects.objects;
        
        if constexpr (Internal::has_EnableConcurrency<SystemType, int()>::value) {
        
            const int chunkSize = ((SystemType*)this)->EnableConcurrency();
               
            for (int i=0; i<gameObjectsInSystem.size(); i+=chunkSize) {
               int fromIndex = i;
               int toIndex = std::min((int)gameObjectsInSystem.size(),  fromIndex + chunkSize);
               
               taskRunner.RunTask([this, &gameObjectsInSystem, &components, this_system, fromIndex, toIndex]() {
                   for(int i = fromIndex; i<toIndex; ++i) {
                       const auto gameObject = gameObjectsInSystem[i];
                       const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
                       const auto iterator = std::tuple_cat(this_system, componentValues);
                       std::apply(&SystemType::Update, iterator);
                   }
               });
            }

            while (taskRunner.Update());
            
        } else {
            for(int i = 0; i<gameObjectsInSystem.size(); ++i) {
                const auto gameObject = gameObjectsInSystem[i];
                const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
                const auto iterator = std::tuple_cat(this_system, componentValues);
                std::apply(&SystemType::Update, iterator);
            }
        }

        for(const auto gameObject : gameObjectsInSystem) {
            this->template ChangeComponents(gameObject, components);
        }
    }
};

}
