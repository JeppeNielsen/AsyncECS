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
#include "ComponentContainer.hpp"
#include "IdHelper.hpp"
#include "Timer.hpp"
#include "Registry.hpp"
#include "System.hpp"
#include "Scene.hpp"

using namespace AsyncECS;

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








struct VelocitySystem : System<Position, const Velocity>,
                        NoDependencies {
    void Initialize() {
        std::cout << "VelocitySystem :: Initialized"<<std::endl;
    }
    
    void Update(Position& position, const Velocity& velocity) {
        position.position.x += velocity.velocity.x;
        position.position.y += velocity.velocity.y;
    }
};

struct BoundingBoxSystem : System<const Position, const Mesh, BoundingBox>,
                           NoDependencies {

    void Initialize() {
        std::cout << "BoundingBoxSystem :: Initialized" << std::endl;
    }

    void Update(const Position& position, const Mesh& mesh, BoundingBox& boundingBox) const {
        boundingBox.min = position.position;
    }

};

struct QuadTreeSystem : System<const BoundingBox>,
                        NoDependencies {

    void Initialize() {
    
    }
    
    void Update(const BoundingBox& box) {
        
    }
};

struct RenderSystem : System<Position, const Renderable, Mesh>,
                      Dependencies<BoundingBoxSystem, VelocitySystem> {

    void Initialize(BoundingBoxSystem& dependency, VelocitySystem& s) {
        std::cout << "RenderSystem :: Initialized" << std::endl;
    }

    void Update(Position& position, const Renderable& renderable, Mesh& mesh) const {
        //std::cout << "RenderSystem::Update "<< &position << "  "<< position.position.x <<std::endl;
    
        position.position.x += renderable.materialId;
        
        for (int i=0; i<1000; ++i) {
            position.position.y += sin(i * sqrt(i/1.0f));
        }
        
    }
};

using AllComponents = ComponentTypes<Position, Renderable, Mesh, BoundingBox, Velocity>;
using AllSystems = SystemTypes<RenderSystem, BoundingBoxSystem, VelocitySystem>;

using RegistryType = Registry<AllComponents>;
using SceneType = Scene<RegistryType, AllSystems>;

const int NUM_ENTITIES = 10000;
const int NUM_ITERATIONS = 1;
const int NUM_TIMINGS = 1;

int main() {

    DebugTemplate<AllSystems::UniqueSystems>();

    RegistryType registry;
    SceneType scene(registry);
    
    using count = std::tuple_size<std::tuple<int, bool,bool>>;
    std::array<int, count::value> array;
    
    int serialTime = 0;
    int parallelTime = 0;
    {
    
        for (int i=0; i<10; ++i) {
        
            auto gameObject = scene.CreateGameObject();
            scene.AddComponent<Position>(gameObject, 128.0f, 3.0f);
            scene.AddComponent<Velocity>(gameObject, 1.0f,2.0f);
        }
        
    
        for (int i=0; i<NUM_ENTITIES; ++i) {
        
            auto gameObject = scene.CreateGameObject();
            //scene.AddComponent<Position>(gameObject, 128.0f, 3.0f);
            //scene.AddComponent<Renderable>(gameObject, 102);
            //scene.AddComponent<Mesh>(gameObject);
            //scene.AddComponent<BoundingBox>(gameObject, 1.0f);
            scene.AddComponent<Velocity>(gameObject, 1.0f,2.0f);
        
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
    
    return EXIT_SUCCESS;
}
