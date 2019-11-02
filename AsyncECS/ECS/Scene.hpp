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
#include <future>
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
    
    void Remove(const GameObject gameObject) {
        assert(registry.IsGameObjectValid(gameObject));
        TupleHelper::Iterate(registry.components, [gameObject, this] (auto& component) {
            using ComponentContainerType = std::remove_reference_t<decltype(component)>;
            using ComponentType = typename ComponentContainerType::Type;
            if (!componentObjects[TupleHelper::Index<ComponentContainerType, Components>::value].Contains(gameObject)) {
                return;
            }
            RemoveComponent<ComponentType>(gameObject);
        });
        gameObjects.Remove(gameObject);
        registry.gameObjects.Remove(gameObject);
    }
    
    template<typename T, typename... Args>
    void AddComponent(const GameObject gameObject, Args&& ... args) {
        static_assert(TupleHelper::HasType<ComponentContainer<T>, Components>::value, "Component type not found");
        assert(registry.IsGameObjectValid(gameObject));
        componentObjects[TupleHelper::Index<ComponentContainer<T>, Components>::value].Add(gameObject);
        std::get<ComponentContainer<T>>(registry.components).Create(gameObject,args...);
    }
    
    template<typename T>
    void RemoveComponent(const GameObject gameObject) {
        static_assert(TupleHelper::HasType<ComponentContainer<T>, Components>::value, "Component type not found");
        assert("GameObject has been removed" && registry.IsGameObjectValid(gameObject));
        componentObjects[TupleHelper::Index<ComponentContainer<T>, Components>::value].Remove(gameObject);
        return std::get<ComponentContainer<T>>(registry.components).Remove(gameObject);
    }
    
    template<typename T>
    T& GetComponent(const GameObject gameObject) {
        static_assert(TupleHelper::HasType<ComponentContainer<T>, Components>::value, "Component type not found");
        assert("GameObject has been removed" && registry.IsGameObjectValid(gameObject));
        return std::get<ComponentContainer<T>>(registry.components).Get(gameObject);
    }
    
    template<typename T>
    const T& GetComponentConst(const GameObject gameObject) const {
        static_assert(TupleHelper::HasType<ComponentContainer<T>, Components>::value, "Component not found");
        assert("GameObject has been removed" && registry.IsGameObjectValid(gameObject));
        return std::get<ComponentContainer<T>>(registry.components).GetConst(gameObject);
    }
    
    void Update() {
        const auto& components = registry.components;
        TupleHelper::Iterate(systems, [&components, this](auto& system) {
            using SystemType = std::remove_reference_t<decltype(system)>;
            system.template Iterate<Components, decltype(componentObjects), SystemType>(components, componentObjects);
        });
        frameCounter++;
        registry.ResetChanged();
    }
    
    void UpdateParallel() {
        const auto& components = registry.components;
        
        auto frameNo = frameCounter;
        
        TupleHelper::Iterate(systems, [&components, frameNo, this](auto& system) {
        
            std::vector<GameObject>& gameObjectsInSystem = system.template GetObjects<Components>(componentObjects);
        
            std::vector<std::future<void>> chunks;
            const int chunkSize = 500;
            
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
