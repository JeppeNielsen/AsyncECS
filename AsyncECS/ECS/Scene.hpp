//
//  Scene.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "IScene.hpp"
#include "Registry.hpp"
#include "SystemDependencies.hpp"
#include "SystemTypes.hpp"
#include "System.hpp"
#include "SystemChanged.hpp"
#include "SystemChangedGameObject.hpp"
#include "ComponentView.hpp"
#include "TupleHelper.hpp"
#include "GameObjectCollection.hpp"
#include <future>
#include <array>
#include "ClassNameHelper.hpp"
#include <map>
#include <set>
#include "SystemTask.hpp"
#include "TaskRunner.hpp"
#include <ostream>
#include "SceneModifier.hpp"
#include "HasMethodHelper.hpp"
#include "RemoveDependencies.hpp"

namespace AsyncECS {
    
template<typename Registry, typename Systems>
class Scene : public IScene {
private:
    using Components = decltype(Registry::components);
    
    Registry& registry;
    
    typename Systems::UniqueSystems systems;
    
    using NumSystemsType = std::tuple_size<decltype(systems)>;
    
    static constexpr decltype(NumSystemsType::value) NumSystems = NumSystemsType::value;
    
    GameObjectCollection gameObjects;
    std::array<GameObjectCollection, Registry::NumComponentsType::value> componentObjects;
    std::array<SystemTask, NumSystems> systemTasks;
    TaskRunner taskRunner;
    GameObjectCollection objectsScheduledForRemoval;
    
    void CreateTasks() {
        TupleHelper::Iterate(systems, [this](auto& system) {
            const auto& components = this->registry.components;
            using SystemType = std::remove_reference_t<decltype(system)>;
            const auto taskIndex = TupleHelper::Index<SystemType, decltype(systems)>::value;
            systemTasks[taskIndex].name = ClassNameHelper::GetName<SystemType>();
            systemTasks[taskIndex].work = [this, &components, &system, taskIndex] () {
                Game::Timer timer;
                timer.Start();
                system.template Iterate<Components, decltype(componentObjects), SystemType>(components, componentObjects);
                systemTasks[taskIndex].lastTime = timer.Stop();
            };
        });
    }
    
    void ConnectTasks() {
        std::map<int, std::set<int>> connections;
        TupleHelper::Iterate(systems, [this, &connections](auto& system) {
            using SystemType = std::remove_reference_t<decltype(system)>;
            
            const auto taskIndex = TupleHelper::Index<SystemType, decltype(systems)>::value;
            
            TupleHelper::Iterate(system.GetComponentTypes(), [this, taskIndex, &connections](auto type) {
                if (!type) return; //only write types, ie non const components
                using ComponentType = std::remove_pointer_t<decltype(type)>;
                
                TupleHelper::Iterate(systems, [this, taskIndex, &connections](auto& innerSystem) {
                    using InnerSystemType = std::remove_reference_t<decltype(innerSystem)>;
                    
                    const auto targetTaskIndex = TupleHelper::Index<InnerSystemType, decltype(systems)>::value;
                    if (taskIndex == targetTaskIndex) return;
                    
                    if (innerSystem.template HasComponentType<const ComponentType>()) {
                        connections[taskIndex].insert(targetTaskIndex);
                    }
                    
                    if (innerSystem.template HasComponentType<ComponentType>()) {
                        connections[taskIndex].insert(targetTaskIndex);
                    }
                    
                    if constexpr (AsyncECS::Internal::has_IsComponentView<InnerSystemType, void()>::value) {
                       if (innerSystem.template HasComponentViewType<const ComponentType>()) {
                           connections[taskIndex].insert(targetTaskIndex);
                       }
                       
                       if (innerSystem.template HasComponentViewType<ComponentType>()) {
                           connections[taskIndex].insert(targetTaskIndex);
                       }
                    }
                });
            });
            
            
            if constexpr (AsyncECS::Internal::has_IsComponentView<SystemType, void()>::value) {
                TupleHelper::Iterate(system.GetComponentViewTypes(), [this, taskIndex, &connections](auto type) {
                    if (!type) return; //only write types, ie non const components
                    using ComponentType = std::remove_pointer_t<decltype(type)>;
                    
                    TupleHelper::Iterate(systems, [this, taskIndex, &connections](auto& innerSystem) {
                        using InnerSystemType = std::remove_reference_t<decltype(innerSystem)>;
                        
                        const auto targetTaskIndex = TupleHelper::Index<InnerSystemType, decltype(systems)>::value;
                        if (taskIndex == targetTaskIndex) return;
                        
                        if (innerSystem.template HasComponentType<const ComponentType>()) {
                            connections[taskIndex].insert(targetTaskIndex);
                        }
                        
                        if (innerSystem.template HasComponentType<ComponentType>()) {
                            connections[taskIndex].insert(targetTaskIndex);
                        }
                        
                        if constexpr (AsyncECS::Internal::has_IsComponentView<InnerSystemType, void()>::value) {
                           if (innerSystem.template HasComponentViewType<const ComponentType>()) {
                               connections[taskIndex].insert(targetTaskIndex);
                           }
                           
                           if (innerSystem.template HasComponentViewType<ComponentType>()) {
                               connections[taskIndex].insert(targetTaskIndex);
                           }
                        }
                    
                    });
                });
            }
            
            if constexpr (AsyncECS::Internal::has_IsSystemDependencies<SystemType, void()>::value) {
                TupleHelper::Iterate(system.GetDependencyTypes(), [this, taskIndex, &connections](auto type) {
                    using DependencySystemType = std::remove_pointer_t<decltype(type)>;
                    const auto targetTaskIndex = TupleHelper::Index<DependencySystemType, decltype(systems)>::value;
                    connections[targetTaskIndex].insert(taskIndex);
                });
            }
        });
        
        TupleHelper::Iterate(systems, [this, &connections](auto& system) {
            using SystemType = std::remove_reference_t<decltype(system)>;
        
            if constexpr (AsyncECS::Internal::has_IsRemoveDependencies<SystemType, void()>::value) {
            
                const auto taskIndex = TupleHelper::Index<SystemType, decltype(systems)>::value;
                
                TupleHelper::Iterate(system.GetDependenciesTypesToRemove(), [this, taskIndex, &connections](auto type) {
                    using DependencySystemType = std::remove_pointer_t<decltype(type)>;
                    const auto targetTaskIndex = TupleHelper::Index<DependencySystemType, decltype(systems)>::value;
                    
                    auto& set = connections[targetTaskIndex];
                    auto it = std::find(set.begin(), set.end(), taskIndex);
                    if (it!=set.end()) {
                        set.erase(it);
                    }
                });
            }
        });
        
        for(auto [taskIndex, targets] : connections) {
            for(auto target : targets) {
                systemTasks[taskIndex].Precede(systemTasks[target]);
            }
        }
    }
    
    void RunTask(SystemTask& task) {
        if (!task.IsReady()) return;
        
        taskRunner.RunTask(task.work, [this, &task] () {
            task.isDone = true;
            //std::cout << task.name << " : " << (task.lastTime * 1000.0f) <<std::endl;
            for(auto outgoingTask : task.outgoing) {
                RunTask(*outgoingTask);
            }
        });
    }

public:
    Scene(Registry& registry) : registry(registry){
    
        using ThisType = std::remove_pointer_t<decltype(this)>;
    
        TupleHelper::Iterate(systems, [&registry, this](auto& system) {
            using SystemType = std::remove_reference_t<decltype(system)>;
            
            if constexpr (AsyncECS::Internal::has_IsSystemDependencies<SystemType, void()>::value) {
                auto this_system = std::make_tuple(&system);
                auto dependencies = system.GetDependenciesReferences(systems);
                auto initializer = std::tuple_cat(this_system, dependencies);
                std::apply(&std::remove_reference_t<decltype(system)>::Initialize, initializer);
            }
            
            if constexpr (AsyncECS::Internal::has_SetComponents<SystemType, void(Components&)>::value) {
               system.SetComponents(registry.components);
            }
            
            if constexpr (AsyncECS::Internal::has_InitializeSceneModifier<SystemType, void(IScene&, Components&)>::value) {
                system.InitializeSceneModifier(*this, registry.components);
            }
        });
        CreateTasks();
        ConnectTasks();
    }

    GameObject CreateGameObject() override {
        auto gameObject = registry.gameObjects.Create();
        gameObjects.Add(gameObject);
        return gameObject;
    }
    
    void RemoveGameObject(const GameObject gameObject) override {
        assert(registry.IsGameObjectValid(gameObject));
        if (objectsScheduledForRemoval.Contains(gameObject)) {
            return;
        }
        objectsScheduledForRemoval.Add(gameObject);
        TupleHelper::Iterate(systems, [gameObject, this](auto& system) {
            using SystemType = std::remove_reference_t<decltype(system)>;
            if constexpr (Internal::has_GameObjectRemoved<SystemType, void(GameObject)>::value) {
                system.GameObjectRemoved(gameObject);
            }
        });
    }
    
    bool IsGameObjectRemoved(const GameObject gameObject) const override {
        return !registry.IsGameObjectValid(gameObject) ||
            objectsScheduledForRemoval.Contains(gameObject);
    }
    
    template<typename T, typename... Args>
    void AddComponent(const GameObject gameObject, Args&& ... args) {
        static_assert(TupleHelper::HasType<ComponentContainer<T>, Components>::value, "Component type not found");
        assert(registry.IsGameObjectValid(gameObject));
        componentObjects[TupleHelper::Index<ComponentContainer<T>, Components>::value].Add(gameObject);
        std::get<ComponentContainer<T>>(registry.components).Create(gameObject,args...);
    }
    
    template<typename T>
    void ReferenceComponent(const GameObject gameObject, const GameObject referenceObject) {
        static_assert(TupleHelper::HasType<ComponentContainer<T>, Components>::value, "Component type not found");
        assert(registry.IsGameObjectValid(gameObject));
        assert(registry.IsGameObjectValid(referenceObject));
        componentObjects[TupleHelper::Index<ComponentContainer<T>, Components>::value].Add(gameObject);
        std::get<ComponentContainer<T>>(registry.components).Reference(gameObject, referenceObject);
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
    
    template<typename T>
    bool HasComponent(const GameObject gameObject) const {
        static_assert(TupleHelper::HasType<ComponentContainer<T>, Components>::value, "Component type not found");
        assert("GameObject has been removed" && registry.IsGameObjectValid(gameObject));
        return componentObjects[TupleHelper::Index<ComponentContainer<T>, Components>::value].Contains(gameObject);
    }
    
    void AddGameObjectToComponentContainer(const GameObject gameObject, const int index) override {
        assert(registry.IsGameObjectValid(gameObject));
        componentObjects[index].Add(gameObject);
    }
    
    void RemoveGameObjectFromComponentContainer(const GameObject gameObject, const int index) override {
        assert(registry.IsGameObjectValid(gameObject));
        componentObjects[index].Remove(gameObject);
    }
    
    template<typename System>
    System& GetSystem() {
        return std::get<System>(systems);
    }
        
    void Update() {
        Game::Timer timer;
        timer.Start();
        for(auto& task : systemTasks) {
            task.isDone = false;
            if (task.incoming.empty()) {
                RunTask(task);
            }
        }
        
        while(taskRunner.Update());
        registry.ResetChanged(); // should be moved out of Scene, because there might be multiple Scenes.
        while (UpdateSceneModifiers());
        RemoveGameObjects();
        //std::cout << "Scene::Update: "<< timer.Stop() * 1000 << "\n";
    }
    
    bool UpdateSceneModifiers() {
        bool anyChange = false;
        using ThisType = std::remove_pointer_t<decltype(this)>;
        TupleHelper::Iterate(systems, [this, &anyChange](auto& system) {
            using SystemType = std::remove_reference_t<decltype(system)>;
            if constexpr (AsyncECS::Internal::has_InitializeSceneModifier<SystemType, void(IScene&, Components&)>::value) {
                anyChange |= system.UpdateFromScene(*this);
            }
        });
        return anyChange;
    }
    
    void RemoveGameObjects() {
        for(auto gameObject : objectsScheduledForRemoval.objects) {
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
        objectsScheduledForRemoval.Clear();
    }
    
    //http://www.webgraphviz.com/
    void WriteGraph(std::ostream& os) {
        os << "digraph Scene { \n";
        os << "rankdir=\"TB\"; \n";
        os << "subgraph cluster_Scene { \n";
        os << "label=\"Scene\"; \n";
        
        for(auto& task : systemTasks) {
            os << "p"<<(&task) << "[label=\""<< task.name <<"\"]; \n";
            
            for(auto outgoing : task.outgoing) {
                os << "p"<<(&task) << " -> " << "p"<<(outgoing)<< ";\n";
            }
        }
    
        os << "} \n";
        os << "} \n";
    }
};

}
