//
//  SystemChanged.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "SystemBase.hpp"
#include "TaskRunner.hpp"
#include "ClassNameHelper.hpp"
#include <iostream>

namespace AsyncECS {

template<typename...T>
struct SystemChanged : SystemBase<T...> {

    TaskRunner taskRunner;

    template<typename Components, typename ComponentObjects, typename SystemType>
    void Iterate(const Components& components, ComponentObjects& componentObjects) {
        const auto this_system = std::make_tuple((SystemType*)this);
        const auto& gameObjectsInSystem = this->template GetObjects<Components>(componentObjects);
        const auto& changedGameObjects = this->template GetChangedObjects<Components>(components);
        
        
        if constexpr (Internal::has_EnableConcurrency<SystemType, int()>::value) {
            
            const int chunkSize = ((SystemType*)this)->EnableConcurrency();
            
            //std::cout << ClassNameHelper::GetName<SystemType>() << ".changedObjects.size() == "<< changedGameObjects.size() << std::endl;
            
            for (int i=0; i<changedGameObjects.size(); i+=chunkSize) {
                int fromIndex = i;
                int toIndex = std::min((int)changedGameObjects.size(),  fromIndex + chunkSize);
                
                taskRunner.RunTask([this, &components, &changedGameObjects, &gameObjectsInSystem, this_system, fromIndex, toIndex]() {
                    for(int i = fromIndex; i<toIndex; ++i) {
                        const auto gameObject = changedGameObjects[i];
                        if (!gameObjectsInSystem.Contains(gameObject)) {
                            continue;
                        }
                        if (!this->template GameObjectContainsAll(gameObject, components)) {
                            continue;
                        }
                        const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
                        const auto iterator = std::tuple_cat(this_system, componentValues);
                        
                        std::apply(&SystemType::Changed, iterator);
                    }
                    for(int i = fromIndex; i<toIndex; ++i) {
                        const auto gameObject = changedGameObjects[i];
                        if (!gameObjectsInSystem.Contains(gameObject)) {
                            continue;
                        }
                        if (!this->template GameObjectContainsAll(gameObject, components)) {
                            continue;
                        }
                        const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
                        const auto iterator = std::tuple_cat(this_system, componentValues);
                        
                        std::apply(&SystemType::Update, iterator);
                    }
                });
            }
            
            while (taskRunner.Update());
                
        } else {
            
            for(int i = 0; i<changedGameObjects.size(); ++i) {
                const auto gameObject = changedGameObjects[i];
                if (!gameObjectsInSystem.Contains(gameObject)) {
                    continue;
                }
                if (!this->template GameObjectContainsAll(gameObject, components)) {
                    continue;
                }
                const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
                const auto iterator = std::tuple_cat(this_system, componentValues);
                
                std::apply(&SystemType::Changed, iterator);
            }
            for(int i = 0; i<changedGameObjects.size(); ++i) {
                const auto gameObject = changedGameObjects[i];
                if (!gameObjectsInSystem.Contains(gameObject)) {
                    continue;
                }
                if (!this->template GameObjectContainsAll(gameObject, components)) {
                    continue;
                }
                const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
                const auto iterator = std::tuple_cat(this_system, componentValues);
                
                std::apply(&SystemType::Update, iterator);
            }
        }
        
        for(const auto gameObject : changedGameObjects) {
            if (!gameObjectsInSystem.Contains(gameObject)) {
                continue;
            }
            if (!this->template GameObjectContainsAll(gameObject, components)) {
                continue;
            }
            this->template ChangeComponents(gameObject, components);
        }
    }
};

}
