//
//  System.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "ComponentContainer.hpp"
#include "GameObjectCollection.hpp"
#include "TupleHelper.hpp"
#include "ComponentView.hpp"
#include "TaskRunner.hpp"
#include <algorithm>
#include "ClassNameHelper.hpp"

namespace AsyncECS {

template<typename ...T>
struct Dependencies {

    template<typename B>
    static constexpr B GetDependencies() {
        return B();
    }

    static constexpr std::tuple<T...> GetDependencies() {
        return std::make_tuple(GetDependencies<T>()...);
    }
    
    template<typename B>
    static constexpr auto GetDependenciesRecursiveInternal() {
        return std::tuple_cat(B::GetDependenciesRecursive(), std::tuple<B>{});
    }
    
    static constexpr auto GetDependenciesRecursive() {
        return std::tuple_cat(GetDependenciesRecursiveInternal<T>()...);
    }
    
    template<typename Systems>
    static std::tuple<T&...> GetDependenciesReferences(Systems& systems) {
        return std::tie((T&)std::get<T>(systems)...);
    }
    
    std::tuple<T*...> GetDependencyTypes() {
        return std::tuple<T*...> {};
    }
};

using NoDependencies = Dependencies<>;

template<typename ...T>
struct SystemTypes {
    
    template<typename System>
    constexpr static auto GetDependency() {
        return std::tuple_cat(System::GetDependenciesRecursive(),std::tuple<System>{});
    }

    constexpr static auto GetDependencies() {
        return std::tuple_cat(GetDependency<T>()...);
    }

    using AllSystems = decltype(GetDependencies());
    using UniqueSystems = TupleHelper::UniqueTypes<AllSystems>;
};


template<typename...T>
struct SystemBase {

    template<typename O>
    std::tuple<O*> GetComponentType(const O* ptr) const {
        return std::make_tuple((O*)nullptr);
    }
    
    template<typename O>
    std::tuple<O*> GetComponentType(O* ptr) const {
        return std::make_tuple((O*)1);
    }

    std::tuple<T*...> GetComponentTypes() {
        return std::tuple_cat(GetComponentType((T*)nullptr)...);
    }
    
    template<typename ComponentType>
    bool HasComponentType() {
        return TupleHelper::HasType<ComponentType, std::tuple<T...> >::value;
    }

    template<typename O, typename Components>
    const O& Get(const GameObject gameObject, Components& components, const O* ptr) const {
        return (const O&)std::get<ComponentContainer<std::remove_const_t<O>>>(components).GetConst(gameObject);
    }
    
    template<typename O, typename Components>
    O& Get(const GameObject gameObject, Components& components, O* ptr) const {
        return (O&)((ComponentContainer<std::remove_const_t<O>>&)std::get<ComponentContainer<std::remove_const_t<O>>>(components)).GetNoChange(gameObject);
    }
   
    template<typename Components>
    std::tuple<T&...> GetComponentValuesFromGameObject(const GameObject gameObject, Components& components) const {
        return std::tie(Get(gameObject, components, (T*)nullptr)...);
    }
    
    template<typename O, typename Components>
    void ChangeComponent(const GameObject gameObject, Components& components, const O* ptr) const {
        /*empty*/ // no change for const components
    }
    
    template<typename O, typename Components>
    void ChangeComponent(const GameObject gameObject, Components& components, O* ptr) const {
        ((ComponentContainer<std::remove_const_t<O>>&)std::get<ComponentContainer<std::remove_const_t<O>>>(components)).SetChanged(gameObject);
    }
    
    template<typename Components>
    void ChangeComponents(const GameObject gameObject, Components& components) const {
        using expander = int[];
        (void) expander { 0, ((void)ChangeComponent(gameObject, components, (T*)nullptr),0)... };
    }
    
    template<typename Components, typename ComponentObjects>
    GameObjectCollection& GetObjects(ComponentObjects& componentObjects) const {
        const int numElements = sizeof...(T);
        
        int indicies[] { TupleHelper::Index<ComponentContainer<std::remove_const_t<std::remove_pointer_t<T>>>, Components>::value... };
        
        int foundIndex = indicies[0];
        int min = (int)componentObjects[foundIndex].objects.size();
        for(int i = 1; i < numElements; ++i) {
            int index = indicies[i];
            int size =  (int)componentObjects[index].objects.size();
            if (size < min) {
                min =  size;
                foundIndex = index;
            }
        }
        return componentObjects[foundIndex];
    }
    
    template<typename Components>
    const GameObjectCollection::Objects& GetChangedObjects(const Components& components) const {
        const int numElements = sizeof...(T);
        
        const GameObjectCollection::Objects* objects[] { &std::get<ComponentContainer<std::remove_const_t<T>>>(components).changedThisFrame.objects... };
        
        int max = (int)objects[0]->size();
        int foundIndex = 0;
        for(int i = 1; i < numElements; ++i) {
            int size = (int)objects[i]->size();
            if (size>max) {
                foundIndex = i;
                max = size;
            }
        }
        return *objects[foundIndex];
    }
};

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

template<typename...T>
struct SystemChanged : SystemBase<T...> {

    TaskRunner taskRunner;

    template<typename Components, typename ComponentObjects, typename SystemType>
    void Iterate(const Components& components, ComponentObjects& componentObjects) {
        const auto this_system = std::make_tuple((SystemType*)this);
        const auto& gameObjectsInSystem = this->template GetObjects<Components>(componentObjects);
        const auto& changedGameObjects = this->template GetChangedObjects<Components>(components);
        
        const int chunkSize = 50000;
        
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
                    const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
                    const auto iterator = std::tuple_cat(this_system, componentValues);
                    std::apply(&SystemType::Update, iterator);
                }
            });
        }
        
        for(const auto gameObject : changedGameObjects) {
            if (!gameObjectsInSystem.Contains(gameObject)) {
                continue;
            }
            this->template ChangeComponents(gameObject, components);
        }
    }
};

template<typename...T>
struct SystemChangedGameObject : SystemBase<T...> {

    TaskRunner taskRunner;

    template<typename Components, typename ComponentObjects, typename SystemType>
    void Iterate(const Components& components, ComponentObjects& componentObjects) {
        const auto this_system = std::make_tuple((SystemType*)this);
        const auto& gameObjectsInSystem = this->template GetObjects<Components>(componentObjects);
        const auto& changedGameObjects = this->template GetChangedObjects<Components>(components);
        
        const int chunkSize = ClassNameHelper::GetName<SystemType>() == "QuadTreeSystem" ? 10000000 : 100000;
        
        //std::cout << ClassNameHelper::GetName<SystemType>() << ".changedObjects.size() == "<< changedGameObjects.size() << std::endl;
        
        for (int i=0; i<changedGameObjects.size(); i+=chunkSize) {
            int fromIndex = i;
            int toIndex = std::min((int)changedGameObjects.size(),  fromIndex + chunkSize);
            
            taskRunner.RunTask([this, &components, &changedGameObjects, &gameObjectsInSystem, &this_system, fromIndex, toIndex]() {
                for(int i = fromIndex; i<toIndex; ++i) {
                    const auto gameObject = changedGameObjects[i];
                    if (!gameObjectsInSystem.Contains(gameObject)) {
                        continue;
                    }
                    const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
                    const auto iterator = std::tuple_cat(this_system, std::tuple_cat( std::make_tuple(gameObject), componentValues ));
                    std::apply(&SystemType::Update, iterator);
                }
            }, []() {});
        }
        
        while(taskRunner.Update());
        
        for(const auto gameObject : changedGameObjects) {
            if (!gameObjectsInSystem.Contains(gameObject)) {
                continue;
            }
            this->template ChangeComponents(gameObject, components);
        }
        
        /*
        for(const auto gameObject : changedGameObjects) {
            if (!gameObjectsInSystem.Contains(gameObject)) {
                continue;
            }
            const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, components);
            const auto iterator = std::tuple_cat(this_system, std::tuple_cat( std::make_tuple(gameObject), componentValues ));
            std::apply(&SystemType::Update, iterator);
        }
        */
    }
};


}
