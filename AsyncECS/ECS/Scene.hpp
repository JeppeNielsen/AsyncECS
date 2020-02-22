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
#include "ClassNameHelper.hpp"
#include <map>
#include <set>
#include "SystemTask.hpp"
#include "TaskRunner.hpp"
#include <ostream>

namespace AsyncECS {
    
template<typename Registry, typename Systems>
struct Scene {
    using Components = decltype(Registry::components);
    
    Registry& registry;
    int frameCounter;
    
    typename Systems::UniqueSystems systems;
    
    using NumSystemsType = std::tuple_size<decltype(systems)>;
    
    static constexpr decltype(NumSystemsType::value) NumSystems = NumSystemsType::value;
    
    GameObjectCollection gameObjects;
    std::array<GameObjectCollection, Registry::NumComponentsType::value> componentObjects;
    std::array<SystemTask, NumSystems> systemTasks;
    TaskRunner taskRunner;
    
    Scene(Registry& registry) : registry(registry), frameCounter(0) {
        TupleHelper::Iterate(systems, [&registry, this](auto& system) {
            auto this_system = std::make_tuple(&system);
            auto dependencies = system.GetDependenciesReferences(systems);
            auto initializer = std::tuple_cat(this_system, dependencies);
            std::apply(&std::remove_reference_t<decltype(system)>::Initialize, initializer);
            system.SetComponents(registry.components);
        });
        CreateTasks();
        ConnectTasks();
    }
    
    void CreateTasks() {
        TupleHelper::Iterate(systems, [this](auto& system) {
            const auto& components = this->registry.components;
            using SystemType = std::remove_reference_t<decltype(system)>;
            const auto taskIndex = TupleHelper::Index<SystemType, decltype(systems)>::value;
            systemTasks[taskIndex].name = ClassNameHelper::GetName<SystemType>();
            systemTasks[taskIndex].work = [this, &components, &system] () {
                system.template Iterate<Components, decltype(componentObjects), SystemType>(components, componentObjects);
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
                    
                    if (innerSystem.template HasComponentType<const ComponentType>() ||
                        innerSystem.template HasComponentViewType<const ComponentType>()) {
                        connections[taskIndex].insert(targetTaskIndex);
                    }
                    
                    if (innerSystem.template HasComponentType<ComponentType>() ||
                        innerSystem.template HasComponentViewType<ComponentType>()) {
                        connections[taskIndex].insert(targetTaskIndex);
                    }
                });
            });
            
            TupleHelper::Iterate(system.GetDependencyTypes(), [this, taskIndex, &connections](auto type) {
                using DependencySystemType = std::remove_pointer_t<decltype(type)>;
                const auto targetTaskIndex = TupleHelper::Index<DependencySystemType, decltype(systems)>::value;
                connections[targetTaskIndex].insert(taskIndex);
            });
        });
        
        for(auto [taskIndex, targets] : connections) {
            for(auto target : targets) {
                systemTasks[taskIndex].Precede(systemTasks[target]);
            }
        }
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
    
   template<typename T>
   bool HasComponent(const GameObject gameObject) const {
        static_assert(TupleHelper::HasType<ComponentContainer<T>, Components>::value, "Component type not found");
        assert("GameObject has been removed" && registry.IsGameObjectValid(gameObject));
        return componentObjects[TupleHelper::Index<ComponentContainer<T>, Components>::value].Contains(gameObject);
    }
    
    template<typename System>
    System& GetSystem() {
        return std::get<System>(systems);
    }
    
    void RunTask(SystemTask& task) {
        if (!task.IsReady()) return;
        
        taskRunner.RunTask(task.work, [this, &task] () {
            task.isDone = true;
            //std::cout << task.name << " - done"<<std::endl;
            for(auto outgoingTask : task.outgoing) {
                RunTask(*outgoingTask);
            }
        });
    }
    
    void Update() {
        for(auto& task : systemTasks) {
            task.isDone = false;
            if (task.incoming.empty()) {
                RunTask(task);
            }
        }
        
        while(taskRunner.Update());
        
        /*
        executor.run(flow);
        executor.wait_for_all();
        */
        
        /*
        const auto& components = this->registry.components;
        TupleHelper::Iterate(systems, [this, &components](auto& system) {
            using SystemType = std::remove_reference_t<decltype(system)>;
            system.template Iterate<Components, decltype(componentObjects), SystemType>(components, componentObjects);
            std::cout << "System : "<< ClassNameHelper::GetName<SystemType>()<<" \n";
        });
        */
        
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
