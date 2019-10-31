//
//  Container.hpp
//  ECS
//
//  Created by Jeppe Nielsen on 25/07/2018.
//  Copyright © 2018 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <vector>
#include <assert.h>
#include "GameObject.hpp"

namespace AsyncECS {

template<typename T>
struct ComponentContainer {

    ComponentContainer() = default;
    ~ComponentContainer() = default;

    ComponentContainer(const ComponentContainer &) = delete;
    ComponentContainer(ComponentContainer &&) = default;

    ComponentContainer & operator=(const ComponentContainer &) = delete;
    ComponentContainer & operator=(ComponentContainer &&) = default;
    
    void CreateIndex(const GameObject gameObject) {
        const auto objectIndex = gameObject & GameObjectIndexMask;
        
        if (objectIndex>=indicies.size()) {
            indicies.resize(objectIndex + 1, GameObjectNull);
        }
        
        indicies[objectIndex] = (std::uint32_t)elements.size();
        references.push_back(1);
        changed.push_back(0);
    }
    
    void Create(const GameObject gameObject) {
        CreateIndex(gameObject);
        elements.resize(elements.size() + 1);
    }
    
    template<typename... Args>
    std::enable_if_t<!std::is_constructible<T, Args...>::value, void>
    Create(const GameObject gameObject, Args&& ... args) {
        CreateIndex(gameObject);
        elements.emplace_back(T{std::forward<Args>(args)...});
    }
    
    template<typename... Args>
    std::enable_if_t<std::is_constructible<T, Args...>::value, void>
    Create(const GameObject gameObject, Args&& ... args) {
        CreateIndex(gameObject);
        elements.emplace_back(std::forward<Args>(args)...);
    }
    
    void Reference(const GameObject gameObject, const GameObject referenceGameObject) {
        const auto objectIndex = gameObject & GameObjectIndexMask;
        
        if (objectIndex>=indicies.size()) {
            indicies.resize(objectIndex + 1, GameObjectNull);
        }
        
        const auto referenceObjectIndex = referenceGameObject & GameObjectIndexMask;
        
        const auto referenceIndex = indicies[referenceObjectIndex];
        
        indicies[objectIndex] = referenceIndex;
        ++references[referenceIndex];
    }
    
    void Destroy(const GameObject gameObject) {
        
        const auto objectIndex = gameObject & GameObjectIndexMask;
        const auto index = indicies[objectIndex];
        
        if ((--references[index])==0) {
            auto tmp = std::move(elements.back());
            elements[index] = std::move(tmp);
            elements.pop_back();
            references[index] = references.back();
            references.pop_back();
            changed[index] = changed.back();
            changed.pop_back();
        }
        
        indicies[objectIndex] = GameObjectNull;
    }
    
    T& Get(const GameObject gameObject, const int changedFrame) {
        auto const elementIndex = indicies[gameObject & GameObjectIndexMask];
        changed[elementIndex] = changedFrame;
        return (T&)elements[elementIndex];
    }
    
    const T& GetConst(const GameObject gameObject) const {
        return (const T&)elements[indicies[gameObject & GameObjectIndexMask]];
    }
    
    std::uint32_t GetChanged(const GameObject gameObject) const {
        return changed[indicies[gameObject & GameObjectIndexMask]];
    }
    
    std::vector<std::uint32_t> indicies;
    std::vector<T> elements;
    std::vector<std::uint32_t> changed;
    std::vector<std::uint16_t> references;
};

}