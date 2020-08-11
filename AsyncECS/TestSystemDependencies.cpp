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
#include "AllTests.hpp"
#include "ComponentContainer.hpp"
#include "Timer.hpp"
#include "Registry.hpp"
#include "System.hpp"
#include "Scene.hpp"
#include <algorithm>
#include <mutex>

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

struct Nameable {
    std::string name;
};

struct BoundingBox {
    float minX;
    float minY;
    float maxX;
    float maxY;
    
    bool Intersect(const BoundingBox& other) const{
        return !(minX > other.maxX
                    || maxX < other.minX
                    || maxY < other.minY
                    || minY > other.maxY);
    }

    bool Inside(const Vector2& position) const{
        return !(position.x < minX ||
                 position.x > maxX ||
                 position.y > maxY ||
                 position.y < minY);
    }
};

struct QuadTreeNode {
    int prevIndex = -1;
};

struct Renderable {
    int materialId;
};

struct Camera {
    Vector2 size;
};

struct VelocitySystem : System<Position, const Velocity> {
    void Update(Position& position, const Velocity& velocity) {
        position.position.x += velocity.velocity.x;
        position.position.y += velocity.velocity.y;
    }
};

struct BoundingBoxSystem : SystemChanged<const Position, const Mesh, BoundingBox> {

    void Changed(const Position& position, const Mesh& mesh, BoundingBox& boundingBox) const {
    
    }

    void Update(const Position& position, const Mesh& mesh, BoundingBox& boundingBox) const {
        if (mesh.vertices.empty()) return;
        
        float minX = mesh.vertices[0].x;
        float minY = mesh.vertices[0].y;
        
        float maxX = mesh.vertices[0].x;
        float maxY = mesh.vertices[0].y;
        
        for(int i = 1; i < mesh.vertices.size();++i) {
            minX = std::min(minX, mesh.vertices[i].x);
            minY = std::min(minY, mesh.vertices[i].y);
            
            maxX = std::max(maxX, mesh.vertices[i].x);
            maxY = std::max(maxY, mesh.vertices[i].y);
        }
    
        boundingBox.minX = position.position.x + minX;
        boundingBox.minY = position.position.y + minY;
        boundingBox.maxX = position.position.x + maxX;
        boundingBox.maxY = position.position.y + maxY;
    }
};

struct QuadTreeSystem : SystemChangedGameObject<const BoundingBox, QuadTreeNode>{
    BoundingBox root;
    Vector2 rootSize;
    static constexpr int GridSize = 10;
    
    std::array<GameObjectCollection, GridSize * GridSize> grid;
    
    std::mutex* mutex;
    
    void TryInsertGameObject(const GameObject gameObject, const BoundingBox& box, QuadTreeNode& node) {
        float midX = (box.minX + box.maxX) * 0.5f;
        float midY = (box.minY + box.maxY) * 0.5f;
        
        int gridX = floorf(midX / rootSize.x);
        int gridY = floorf(midY / rootSize.y);
        gridX = std::clamp(gridX, 0, GridSize - 1);
        gridY = std::clamp(gridY, 0, GridSize - 1);
        int index = gridX + gridY * GridSize;
        
        if (index == node.prevIndex) {
            return;
        }
        
        if (node.prevIndex != -1) {
            auto& prevCollection = grid[node.prevIndex];
            if (prevCollection.Contains(gameObject)) {
                prevCollection.Remove(gameObject);
            }
        }
        
        auto& collection = grid[index];
        if (!collection.Contains(gameObject)) {
            //std::lock_guard guard(*mutex);
            collection.Add(gameObject);
            node.prevIndex = index;
        }
        
        std::cout << "Inserted " << gameObject<<std::endl;
    }
    
    QuadTreeSystem() {
        root.minX = 0;
        root.minY = 0;
        root.maxX = 1000;
        root.maxY = 1000;
        
        rootSize.x = root.maxX - root.minX;
        rootSize.y = root.maxY - root.minY;
        
        std::cout << "QuadtreeSystem :: Initialized" << std::endl;
        
        mutex = new std::mutex();
    }
    
    void Update(const GameObject gameObject, const BoundingBox& box, QuadTreeNode& node) {
        TryInsertGameObject(gameObject, box, node);
    }
    
    void GetObjectsInView(BoundingBox& box, GameObjectCollection& foundObjects) {
        int minX = floorf(box.minX / rootSize.x)*GridSize;
        int minY = floorf(box.minY / rootSize.y)*GridSize;
        
        int maxX = floorf(box.maxX / rootSize.x)*GridSize;
        int maxY = floorf(box.maxY / rootSize.y)*GridSize;
        
        minX = std::clamp(minX, 0, GridSize - 1);
        minY = std::clamp(minY, 0, GridSize - 1);
        
        maxX = std::clamp(maxX, 0, GridSize - 1);
        maxY = std::clamp(maxY, 0, GridSize - 1);
        
        for(int x = minX; x<=maxX; ++x) {
            for(int y = minY; y<=maxY; ++y) {
                int index = x + y * GridSize;
                auto& collection = grid[index];
                for(auto go : collection.objects) {
                    foundObjects.Add(go);
                }
            }
        }
    }
};

struct NameableSystem : System<Nameable> {
    void Update(Nameable& nameable) {
        nameable.name = "No way";
    }
};

struct RenderSystem : System<const Position, const Camera>,
                      SystemDependencies<QuadTreeSystem>,
                      ComponentView<const Nameable, const Renderable> {
                      
    QuadTreeSystem* quadTreeSystem;
    GameObjectCollection foundObjects;

    void Initialize(QuadTreeSystem& quadTreeSystem) {
        this->quadTreeSystem = &quadTreeSystem;
    }

    void Update(const Position& position, const Camera& camera) {
        //std::cout << "RenderSystem::Update "<< &position << "  "<< position.position.x <<std::endl;
        BoundingBox box;
        box.minX = position.position.x - camera.size.x;
        box.minY = position.position.y - camera.size.y;
        box.maxX = position.position.x + camera.size.x;
        box.maxY = position.position.y + camera.size.y;
        
        foundObjects.Clear();
        
        quadTreeSystem->GetObjectsInView(box, foundObjects);
        
        for(auto go : foundObjects.objects) {
            GetComponents(go, [] (const Nameable& nameable, const Renderable& renderable) {
                std::cout << " render: "<<nameable.name << " ";
            });
        }
        std::cout << std::endl;
    }
};

using AllComponents = ComponentTypes<Position, Renderable, Mesh, BoundingBox, Velocity, QuadTreeNode, Camera, Nameable>;
using AllSystems = SystemTypes<RenderSystem, BoundingBoxSystem, VelocitySystem, QuadTreeSystem, NameableSystem>;

using RegistryType = Registry<AllComponents>;
using SceneType = Scene<RegistryType, AllSystems>;

int main_complex() {
    //AllTests tests;
    //tests.Run();
    
    std::cout << "Number of cores available : " << std::thread::hardware_concurrency() << "\n";
    
    RegistryType registry;
    SceneType scene(registry);
    
    for (int x=0; x<1; x++) {
        for(int y=0; y<10; y++) {
            auto object = scene.CreateGameObject();
            scene.AddComponent<Position>(object, x*10.0f, y*10.0f);
            scene.AddComponent<Mesh>(object);
            auto& mesh = scene.GetComponent<Mesh>(object);
            mesh.vertices = { {0,0}, {10,0}, {10,10}, {0,10}};
            mesh.indicies = {0,1,2,0,2,3};
            scene.AddComponent<BoundingBox>(object);
            scene.AddComponent<QuadTreeNode>(object);
            
            if (x == 0) {
                std::stringstream ss;
                ss << x << " - " << y;
                scene.AddComponent<Nameable>(object, ss.str());
                
                if (y == 0 || y==10) {
                    scene.AddComponent<Renderable>(object);
                }
            }
        }
    }
    
    auto cameraGo = scene.CreateGameObject();
    scene.AddComponent<Camera>(cameraGo, Vector2 {5.0f,5.0f});
    scene.AddComponent<Position>(cameraGo, 100.0f, 100.0f);
    
    {
        Game::Timer timer;
        scene.Update();
        std::cout << timer.Stop() << std::endl;
    }
    {
        Game::Timer timer;
        scene.Update();
        std::cout << timer.Stop() << std::endl;
    }
    
    std::cout << "Graph: "<<std::endl;
    scene.WriteGraph(std::cout);
    return EXIT_SUCCESS;
}
