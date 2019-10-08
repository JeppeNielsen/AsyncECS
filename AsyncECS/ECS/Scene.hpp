//
//  Scene.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "GameObject.hpp"
#include "Registry.hpp"
#include "System.hpp"
#include "TupleHelper.hpp"
#include "GameObjectCollection.hpp"

namespace AsyncECS {
    
template<typename Registry, typename Systems>
struct Scene {

    Registry& registry;
    
    typename Systems::UniqueSystems systems;
    GameObjectCollection gameObjects;
    
    Scene(Registry& registry) : registry(registry) {
    
        TupleHelper::Iterate(systems, [this](auto& system) {
            auto this_system = std::make_tuple(&system);
            auto dependencies = system.GetDependenciesReferences(systems);
            auto initializer = std::tuple_cat(this_system, dependencies);
            std::apply(&std::remove_reference_t<decltype(system)>::Initialize, initializer);
        });
    }

    GameObject CreateGameObject() {
        auto gameObject = registry.gameObjects.Create();
        gameObjects.Add(gameObject);
        return gameObject;
    }
    
    template<typename T, typename... Args>
    void AddComponent(const GameObject gameObject, Args&& ... args) {
        std::get<ComponentContainer<T>>(registry.components).Create(gameObject,args...);
    }
    
    template<typename T>
    T& GetComponent(const GameObject gameObject) {
        return std::get<ComponentContainer<T>>(registry.components).Get(gameObject);
    }
    
    void Update() {
        const auto& components = registry.components;
        TupleHelper::Iterate(systems, [&components, this](auto& system) {
            auto this_system = std::make_tuple(&system);
            size_t size = gameObjects.objects.size();
            for(size_t i = 0; i < size;++i) {
                auto componentValues = system.GetComponentValuesFromGameObject(gameObjects.objects[i], components);
                auto iterator = std::tuple_cat(this_system, componentValues);
                std::apply(&std::remove_reference_t<decltype(system)>::Update, iterator);
            }
        });
    }
    
    void UpdateParallel() {
        const auto& components = registry.components;
        std::vector<GameObject>& gameObjectsInSystem = gameObjects.objects;
        TupleHelper::Iterate(systems, [&gameObjectsInSystem, &components](auto& system) {
        
            std::vector<std::future<void>> chunks;
            const int chunkSize = 30;
            
            auto this_system = std::make_tuple(&system);
            auto size = gameObjectsInSystem.size();
            
            for(size_t startIndex = 0; startIndex < size; startIndex+=chunkSize) {
                size_t endIndex = startIndex + chunkSize;
                if (endIndex >= size) {
                    endIndex = size;
                }
                
                chunks.push_back(std::async(std::launch::async, [&gameObjectsInSystem, &components, &this_system, &system, startIndex, endIndex]() {
                    for(size_t i = startIndex; i<endIndex; ++i) {
                        auto componentValues = system.GetComponentValuesFromGameObject(gameObjectsInSystem[i], components);
                        auto iterator = std::tuple_cat(this_system, componentValues);
                        std::apply(&std::remove_reference_t<decltype(system)>::Update, iterator);
                    }
                }));
            }
            
            for(const auto& chunk : chunks) {
                chunk.wait();
            }
        });
    }
    
    /*template<typename T, typename... Args>
    std::enable_if_t<std::is_constructible<T, Args...>::value, void>
    void AddComponent(const GameObject gameObject, Args&& ... args) {
        std::get<T>(registry.components).Create(gameObject.index, std::forward<Args...>(args...));
    }*/
    
};

}
