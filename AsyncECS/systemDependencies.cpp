//
//  systemDependencies.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 24/09/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include <iostream>
#include <vector>
#include <future>
#include <sstream>
#include <cmath>
#include "TupleHelper.hpp"
#include "ComponentContainer.hpp"
#include "IdHelper.hpp"
#include "Timer.hpp"

template <typename T> void DebugTemplate() { std::cout << __PRETTY_FUNCTION__ << std::endl;  }


struct Vector2 {
    float x;
    float y;
};

struct Position {
    Vector2 position;
};

struct Velocity {
    Vector2 velocity;
};

struct Mesh {
    std::vector<Vector2> vertices;
    std::vector<int> indicies;
};

struct BoundingBox {
    Vector2 min;
    Vector2 max;
};

struct Renderable {
    int materialId;
};

struct Entity {
    int index;
};

struct EntityCollection {
    std::vector<Entity> entities;
    int idCounter = 0;

    Entity CreateEntity() {
        Entity entity { idCounter++ };
        entities.push_back(entity);
        return entity;
    }
};


using ComponentIdHelper = IdHelper<struct ComponentId>;

template<typename ...T>
using ComponentTypes = std::tuple<ComponentContainer<T>...>;

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
    using UniqueSystems = TupleHelper::without_duplicates<AllSystems>;
};

template<typename Components>
struct Context {
    Components components;
    EntityCollection entities;
    
    template<typename Component>
    Component& GetComponent(Entity entity) {
        return std::get<Component>(components).Get(entity.index);
    }
    
    template<typename Component>
    void AddComponent(Entity entity) {
        std::get<Component>(components).Create(entity.index);
    }
    
};

struct World;

template<typename...T>
struct System {
   
    template<typename Components>
    std::tuple<T&...> GetComponentValuesFromEntity(const int index, Components& components) const {
        return std::tie((T&)std::get<ComponentContainer<std::remove_const_t<T>>>(components).Get(index)...);
    }
   
};


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



template<typename Context, typename Systems>
struct Scene {

    Context& context;
    
    typename Systems::UniqueSystems systems;
    std::vector<Entity> entities;
    
    Scene(Context& context) : context(context) {
    
        TupleHelper::Iterate(systems, [this](auto& system) {
            auto this_system = std::make_tuple(&system);
            auto dependencies = system.GetDependenciesReferences(systems);
            auto initializer = std::tuple_cat(this_system, dependencies);
            std::apply(&std::remove_reference_t<decltype(system)>::Initialize, initializer);
        });
    }

    Entity CreateEntity() {
        auto entity = context.entities.CreateEntity();
        entities.push_back(entity);
        return entity;
    }
    
    template<typename T, typename... Args>
    void AddComponent(const Entity entity, Args&& ... args) {
        std::get<ComponentContainer<T>>(context.components).Create(entity.index,args...);
    }
    
    template<typename T>
    T& GetComponent(const Entity entity) {
        return std::get<ComponentContainer<T>>(context.components).Get(entity.index);
    }
    
    void Update() {
        const auto& components = context.components;
        TupleHelper::Iterate(systems, [&components, this](auto& system) {
            auto this_system = std::make_tuple(&system);
            size_t size = entities.size();
            for(size_t i = 0; i < size;++i) {
                auto componentValues = system.GetComponentValuesFromEntity(entities[i].index, components);
                auto iterator = std::tuple_cat(this_system, componentValues);
                std::apply(&std::remove_reference_t<decltype(system)>::Update, iterator);
            }
        });
    }
    
    void UpdateParallel() {
        const auto& components = context.components;
        std::vector<Entity>& entitiesInSystem = entities;
        TupleHelper::Iterate(systems, [&entitiesInSystem, &components](auto& system) {
        
            std::vector<std::future<void>> chunks;
            const int chunkSize = 30;
            
            auto this_system = std::make_tuple(&system);
            auto size = entitiesInSystem.size();
            
            for(size_t startIndex = 0; startIndex < size; startIndex+=chunkSize) {
                size_t endIndex = startIndex + chunkSize;
                if (endIndex >= size) {
                    endIndex = size;
                }
                
                chunks.push_back(std::async(std::launch::async, [&entitiesInSystem, &components, &this_system, &system, startIndex, endIndex]() {
                    for(size_t i = startIndex; i<endIndex; ++i) {
                        auto componentValues = system.GetComponentValuesFromEntity(entitiesInSystem[i].index, components);
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
    void AddComponent(const Entity entity, Args&& ... args) {
        std::get<T>(context.components).Create(entity.index, std::forward<Args...>(args...));
    }*/
    
};

struct VelocitySystem : System<Position, const Velocity>, Dependencies<> {
    
    void Initialize() {
        std::cout << "VelocitySystem :: Initialized"<<std::endl;
        number = 123;
    }
    
    void Update(Position& position, const Velocity& velocity) {
        position.position.x += velocity.velocity.x;
        position.position.y += velocity.velocity.y;
    }
    
    int number;
    
};

struct BoundingBoxSystem : System<const Position, BoundingBox>,
                           Dependencies<VelocitySystem> {

    void Initialize(VelocitySystem& system) {
        std::cout << "BoundingBoxSystem :: Initialized" << system.number <<std::endl;
    }

    void Update(const Position& position, BoundingBox& boundingBox) const {
        boundingBox.min = position.position;
    }

};

struct RenderSystem : System<Position, const Renderable, Mesh>,
                      Dependencies<BoundingBoxSystem, VelocitySystem> {

    void Initialize(BoundingBoxSystem& dependency, VelocitySystem& s) {
        std::cout << "RenderSystem :: Initialized" << s.number <<std::endl;
    }

    void Update(Position& position, const Renderable& renderable, Mesh& mesh) const {
        //std::cout << "RenderSystem::Update "<< &position << "  "<< position.position.x <<std::endl;
    
        position.position.x += renderable.materialId;
        
        for (int i=0; i<10000; ++i) {
            position.position.y += sin(i * sqrt(i/1.0f));
        }
        
    }
    
};

using AllComponents = ComponentTypes<Position, Renderable, Mesh, BoundingBox, Velocity>;
using AllSystems = SystemTypes<RenderSystem, BoundingBoxSystem, VelocitySystem>;

using ContextType = Context<AllComponents>;
using SceneType = Scene<ContextType, AllSystems>;

const int NUM_ENTITIES = 1000;
const int NUM_ITERATIONS = 1;
const int NUM_TIMINGS = 1;



int main() {

    DebugTemplate<AllSystems::UniqueSystems>();

    ContextType context;
    SceneType scene(context);
    
    
    int serialTime = 0;
    int parallelTime = 0;
    {
        for (int i=0; i<NUM_ENTITIES; ++i) {
        
            auto entity = scene.CreateEntity();
            scene.AddComponent<Position>(entity, 128.0f, 3.0f);
            scene.AddComponent<Renderable>(entity, 102);
            scene.AddComponent<Mesh>(entity);
            scene.AddComponent<BoundingBox>(entity, 1.0f);
            scene.AddComponent<Velocity>(entity, 1.0f,2.0f);
        
        }
        
        {
            int totalTime = 0;
            for (int j=0; j<NUM_TIMINGS; ++j)
            {
                Timer timer;
                for(int i=0; i<NUM_ITERATIONS; ++i) {
                    scene.Update();
                    //StaticUpdate(context, std::get<0>(scene.systems), scene.entities);// scene.entities.size());
                    //for (int k=0; k<NUM_ENTITIES; k++) {
                    //    ren.Update(positions.Get(k), renderables.Get(k), meshes.Get(k));
                    //}
                }
                totalTime+=timer.Stop();
            }
            serialTime =(totalTime / NUM_TIMINGS);
            std::cout << "scene.Update() " << serialTime << std::endl;
        }
        
        int totalTime = 0;
        for (int j=0; j<NUM_TIMINGS; ++j)
        {
            Timer timer;
            for(int i=0; i<NUM_ITERATIONS; ++i) {
                scene.UpdateParallel();
                //StaticUpdate(context, std::get<0>(scene.systems), scene.entities);// scene.entities.size());
                //for (int k=0; k<NUM_ENTITIES; k++) {
                //    ren.Update(positions.Get(k), renderables.Get(k), meshes.Get(k));
                //}
            }
            totalTime+=timer.Stop();
        }
        parallelTime =(totalTime / NUM_TIMINGS);
        std::cout << "scene.UpdateParallel() " << parallelTime << std::endl;
    
    }
    
    std::cout << "Serial/Parallel ratio: " << serialTime / (float)parallelTime << std::endl;
    
   
    /*
    {
        Timer timer("UpdateParallel");
        for(int i=0; i<500; ++i) {
            scene.UpdateParallel();
        }
    }
    */
    
    RenderSystem renderSystem;// =  std::get<1>(scene.systems);
    
    {
        
        ComponentContainer<Position>& positions = std::get<0>(scene.context.components);
        ComponentContainer<Renderable>& renderables = std::get<1>(scene.context.components);
        ComponentContainer<Mesh>& meshes = std::get<2>(scene.context.components);
        
        /*
        for(int i=0; i<NUM_ENTITIES; ++i) {
            positions.Create(i);
            renderables.Create(i);
            meshes.Create(i);
        }
        */
    
        int totalTime = 0;
        for (int i=0; i<NUM_TIMINGS; ++i)
        {
           
            Timer timer;
            
            for(int o=0; o<NUM_ITERATIONS; ++o) {
                for (int i=0; i<NUM_ENTITIES; ++i) {
                    //auto& pos = positions.Get(i);
                    //auto& renderable = renderables.Get(i);
                    //auto& mesh = meshes.Get(i);
                    //renderSystem.Update(pos, renderable, mesh);
                    renderSystem.Update(positions.Get(i), renderables.Get(i), meshes.Get(i));
                }
            }
            
            totalTime += timer.Stop();
            
        }
        
        std::stringstream s;
        s<<(totalTime / NUM_TIMINGS);
        std::cout << "ComponentContainer " << s.str() << std::endl;
        
    }
    
    /*
    std::vector<Position> vector(NUM_ENTITIES);
    std::vector<Renderable> vector2(NUM_ENTITIES);
    std::vector<Mesh> vector3(NUM_ENTITIES);
    
    {
        int totalTime = 0;
        for (int i=0; i<NUM_TIMINGS; ++i)
        {
            Timer timer;
            for(int o=0; o<NUM_ITERATIONS; ++o) {
                for (int i=0; i<NUM_ENTITIES; ++i) {
                    auto& pos = vector[i];
                    auto& renderable = vector2[i];
                    auto& mesh = vector3[i];
                
                    renderSystem.Update(pos, renderable, mesh);
                }
            }
            totalTime+=timer.Stop();
        }
        std::stringstream s;
        s<<(totalTime / NUM_TIMINGS);
        std::cout << "Vector " << s.str() << std::endl;
    }
    */
    
    
    
    return 0;
}
