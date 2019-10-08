//
//  Registry.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <tuple>
#include "GameObjectDatabase.hpp"
#include "ComponentContainer.hpp"

namespace AsyncECS {

template<typename ...T>
using ComponentTypes = std::tuple<ComponentContainer<T>...>;

template<typename Components>
struct Registry {
    Components components;
    GameObjectDatabase gameObjects;
    
    template<typename Component>
    Component& GetComponent(GameObject gameObject) {
        return std::get<Component>(components).Get(gameObject);
    }
    
    template<typename Component>
    void AddComponent(GameObject gameObject) {
        std::get<Component>(components).Create(gameObject);
    }
    
};
  
}

