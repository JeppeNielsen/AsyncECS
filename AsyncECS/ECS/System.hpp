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

    template<typename O, typename Components>
    const O& Get(const int index, const int changedFrame, Components& components, const O* ptr) const {
        return (const O&)std::get<ComponentContainer<std::remove_const_t<O>>>(components).GetConst(index);
    }
    
    template<typename O, typename Components>
    O& Get(const int index, const int changedFrame, Components& components, O* ptr) const {
        return (O&)((ComponentContainer<std::remove_const_t<O>>&)std::get<ComponentContainer<std::remove_const_t<O>>>(components)).Get(index, changedFrame);
    }
   
    template<typename Components>
    std::tuple<T&...> GetComponentValuesFromGameObject(const int index, const int changedFrame, Components& components) const {
        return std::tie(Get(index, changedFrame, components, (T*)nullptr)...);
    }
    
    template<typename Components, typename ComponentObjects>
    GameObjectCollection::Objects& GetObjects(ComponentObjects& componentObjects) const {
        const int numElements = sizeof...(T);
        
        int indicies[] { TupleHelper::Index<ComponentContainer<std::remove_const_t<std::remove_pointer_t<T>>>, Components>::value... };
        int sizes[] { (int)componentObjects[TupleHelper::Index<ComponentContainer<std::remove_const_t<std::remove_pointer_t<T>>>, Components>::value].objects.size()... };
        
        int min = sizes[0];
        int foundIndex = indicies[0];
        for(int i = 1; i < numElements; ++i) {
            if (sizes[i] < min) {
                min = sizes[i];
                foundIndex = indicies[i];
            }
        }
        return componentObjects[foundIndex].objects;
    }
};

template<typename...T>
struct System : SystemBase<T...> {
    
    template<typename Components, typename ComponentObjects, typename SystemType>
    void Iterate(const int frameCounter, const Components& components, ComponentObjects& componentObjects) {
        const auto this_system = std::make_tuple((SystemType*)this);
        const auto& gameObjectsInSystem = this->template GetObjects<Components>(componentObjects);
        for(auto gameObject : gameObjectsInSystem) {
            const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, frameCounter, components);
            const auto iterator = std::tuple_cat(this_system, componentValues);
            std::apply(&SystemType::Update, iterator);
        }
    }
};

template<typename...T>
struct SystemChanged : SystemBase<T...> {

    template<typename O, typename Components>
    bool IsChanged(const GameObject gameObject, const int changedFrame, const Components& components, const O* ptr) const {
        return std::get<ComponentContainer<std::remove_const_t<O>>>(components).GetChanged(gameObject) == changedFrame;
    }
    
    template<typename O, typename Components>
    bool IsChanged(const GameObject gameObject, const int changedFrame, const Components& components, O* ptr) const {
        return false;
    }
   
    template<typename Components>
    bool AnyComponentChanged(const GameObject gameObject, const int changedFrame, const Components& components) const {
       const int numElements = sizeof...(T);
       bool changed[] { IsChanged(gameObject, changedFrame, components, (T*)nullptr)... };
       for(int i=0; i<numElements; ++i) {
           if (changed[i]) return true;
       }
       return false;
    }
    
    template<typename Components, typename ComponentObjects, typename SystemType>
    void Iterate(const int frameCounter, const Components& components, ComponentObjects& componentObjects) {
        const auto this_system = std::make_tuple((SystemType*)this);
        const auto& gameObjectsInSystem = this->template GetObjects<Components>(componentObjects);
        for(auto gameObject : gameObjectsInSystem) {
            if (!AnyComponentChanged(gameObject, frameCounter, components)) continue;
            const auto componentValues = this->template GetComponentValuesFromGameObject(gameObject, frameCounter, components);
            const auto iterator = std::tuple_cat(this_system, componentValues);
            std::apply(&SystemType::Update, iterator);
        }
    }
};

}
