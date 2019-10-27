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
#include <array>

namespace AsyncECS {
    
template<typename Registry, typename Systems>
struct Scene {
    using Components = decltype(Registry::components);
    
    Registry& registry;
    int frameCounter;
    
    typename Systems::UniqueSystems systems;
    GameObjectCollection gameObjects;
    std::array<GameObjectCollection, Registry::NumComponentsType::value> componentObjects;
    
    Scene(Registry& registry) : registry(registry), frameCounter(0) {
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
        componentObjects[TupleHelper::Index<ComponentContainer<T>, Components>::value].Add(gameObject);
        std::get<ComponentContainer<T>>(registry.components).Create(gameObject,args...);
    }
    
    template<typename T>
    T& GetComponent(const GameObject gameObject) {
        return std::get<ComponentContainer<T>>(registry.components).Get(gameObject);
    }
    
    template<typename T>
    void RemoveComponent(const GameObject gameObject) {
        componentObjects[TupleHelper::Index<ComponentContainer<T>, Components>::value].Remove(gameObject);
        return std::get<ComponentContainer<T>>(registry.components).Remove(gameObject);
    }
    
    void Update() {
        const auto& components = registry.components;
        TupleHelper::Iterate(systems, [&components, this](auto& system) {
            using systemType = std::remove_reference_t<decltype(system)>;
        
            const auto this_system = std::make_tuple(&system);
            
            auto index = system.template GetContainerIndex<Components>(componentObjects);
            
            auto& gameObjectsInSystem = componentObjects[index].objects;
            size_t size = gameObjectsInSystem.size();
            
            for(int i = 0; i < size; ++i) {
                const auto componentValues = system.GetComponentValuesFromGameObject(gameObjectsInSystem[i], frameCounter, components);
                const auto iterator = std::tuple_cat(this_system, componentValues);
                std::apply(&systemType::Update, iterator);
            }
        });
        frameCounter++;
    }
    
    void UpdateParallel() {
        return;
        const auto& components = registry.components;
        std::vector<GameObject>& gameObjectsInSystem = gameObjects.objects;
        auto frameNo = frameCounter;
        
        TupleHelper::Iterate(systems, [&gameObjectsInSystem, &components, frameNo](auto& system) {
        
            std::vector<std::future<void>> chunks;
            const int chunkSize = 30;
            
            auto this_system = std::make_tuple(&system);
            auto size = gameObjectsInSystem.size();
           
            
            for(size_t startIndex = 0; startIndex < size; startIndex+=chunkSize) {
                size_t endIndex = startIndex + chunkSize;
                if (endIndex >= size) {
                    endIndex = size;
                }
                
                chunks.push_back(std::async(std::launch::async, [&gameObjectsInSystem, &components, &this_system, &system, startIndex, endIndex, frameNo]() {
                    for(size_t i = startIndex; i<endIndex; ++i) {
                        auto componentValues = system.GetComponentValuesFromGameObject(gameObjectsInSystem[i], frameNo, components);
                        auto iterator = std::tuple_cat(this_system, componentValues);
                        std::apply(&std::remove_reference_t<decltype(system)>::Update, iterator);
                    }
                }));
            }
            
            for(const auto& chunk : chunks) {
                chunk.wait();
            }
        });
        frameCounter++;
    }
};

}
