//
//  ComponentView.h
//  AsyncECS
//
//  Created by Jeppe Nielsen on 06/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <array>
#include "ComponentContainer.hpp"
#include "TupleHelper.hpp"
#include "IScene.hpp"
#include <iostream>
#include "HasMethodHelper.hpp"

namespace AsyncECS {

    namespace Internal {
        HAS_METHOD(InitializeSceneModifier)
    }

    template<typename ...T>
    class SceneModifier {
    public:
        using Modifier = SceneModifier<T...>;
    private:
        using Components = std::tuple<ComponentContainer<std::remove_const_t<T>>*...>;
        std::array<int, sizeof...(T)> componentIndicies;
        
        Components componentsPtrs;
        
        std::vector<std::function<void(Modifier&)>> actions;
        IScene* scene;
        
    public:
        template<typename Components>
        void InitializeSceneModifier(IScene& scene, Components& components) {
            this->scene = &scene;
            int indexCounter = 0;
            TupleHelper::Iterate(componentsPtrs, [this, &components, &indexCounter](auto ptr) {
                using ComponentType = std::remove_const_t<std::remove_pointer_t<decltype(ptr)>>;
                std::get<decltype(ptr)>(componentsPtrs) = &std::get<ComponentType>(components);
                componentIndicies[indexCounter] = TupleHelper::Index<ComponentType, Components>::value;
                indexCounter++;
            });
        }
        
        template<typename Scene>
        bool UpdateFromScene(Scene& scene) {
            if (actions.empty()) {
                return false;
            }
            for(auto& action : actions) {
                action(*this);
            }
            actions.clear();
            return true;
        }
        
        template<typename Registry, typename Systems>
        friend class Scene;
    
        void Modify(std::function<void(Modifier&)>&& action) {
            actions.emplace_back(action);
        }
    
        GameObject CreateGameObject() const {
            return scene->CreateGameObject();
        }
        
        bool IsGameObjectRemoved(const GameObject gameObject) const {
            return scene->IsGameObjectRemoved(gameObject);
        }
        
        void RemoveGameObject(const GameObject gameObject) const {
            scene->RemoveGameObject(gameObject);
        }
        
        template<typename Component, typename... Args>
        void AddComponent(const GameObject gameObject, Args&& ... args) {
            static_assert(TupleHelper::HasType<ComponentContainer<Component>*, Components>::value, "Component type not found");
            scene->AddGameObjectToComponentContainer(gameObject, componentIndicies[TupleHelper::Index<ComponentContainer<Component>*, Components>::value]);
            std::get<ComponentContainer<Component>*>(componentsPtrs)->Create(gameObject,args...);
        }
        
        template<typename Component>
        void RemoveComponent(const GameObject gameObject) {
            static_assert(TupleHelper::HasType<ComponentContainer<Component>*, Components>::value, "Component type not found");
            scene->RemoveGameObjectFromComponentContainer(gameObject, componentIndicies[TupleHelper::Index<ComponentContainer<Component>*, Components>::value]);
            std::get<ComponentContainer<Component>*>(componentsPtrs)->Remove(gameObject);
        }
        
        template<typename Component>
        Component& GetComponent(const GameObject gameObject) {
            static_assert(TupleHelper::HasType<ComponentContainer<Component>*, Components>::value, "Component type not found");
            return std::get<ComponentContainer<Component>*>(componentsPtrs)->Get(gameObject);
        }
    };
}
