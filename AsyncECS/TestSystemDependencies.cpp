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
    float left;
    float top;
    float right;
    float bottom;
    
    bool Intersect(const BoundingBox& other) const{
        return !(left > other.right
                    || right < other.left
                    || top < other.bottom
                    || bottom > other.top);
    }

    bool Inside(const Vector2& position) const{
        return !(position.x < left ||
                 position.x > right ||
                 position.y > top ||
                 position.y < bottom);
    }
};

struct QuadTreeNode {
    
};

struct Renderable {
    int materialId;
};

struct Touchable {
    bool Down[10];
    bool Up[10];
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

struct BoundingBoxSystem : SystemChanged<const Position, const Mesh, BoundingBox>,
                           NoDependencies {

    void Initialize() {
        std::cout << "BoundingBoxSystem :: Initialized" << std::endl;
    }

    void Update(const Position& position, const Mesh& mesh, BoundingBox& boundingBox) const {
        if (mesh.vertices.empty()) return;
        
        float minX = mesh.vertices[0].x;
        float minY = mesh.vertices[0].y;
        
        float maxX = mesh.vertices[0].x;
        float maxY = mesh.vertices[0].y;
        
        for(int i=1; i<mesh.vertices.size();++i) {
            minX = std::min(minX, mesh.vertices[i].x);
            minY = std::min(minY, mesh.vertices[i].y);
            
            maxX = std::max(maxX, mesh.vertices[i].x);
            maxY = std::max(maxY, mesh.vertices[i].y);
        }
    
        boundingBox.left = position.position.x + minX;
        boundingBox.bottom = position.position.y + minY;
        boundingBox.right = position.position.x + maxX;
        boundingBox.top = position.position.y + maxY;
    }
};

struct QuadTreeSystem : SystemChanged<const BoundingBox, QuadTreeNode>,
                        Dependencies<BoundingBoxSystem> {
    
    /*
    struct Node {
        BoundingBox box;
    };
    
    Node root;
    */
    
    BoundingBox root;
    int numInserted;
    std::vector<QuadTreeNode> objects;
    
    void Initialize(BoundingBoxSystem& boundingBoxSystem) {
        root.left = 0;
        root.bottom = 0;
        root.right = 1000;
        root.top = 1000;
        numInserted = 0;
        std::cout << "QuadtreeSystem :: Initialized" << std::endl;
    }
    
    void Update(const BoundingBox& box, QuadTreeNode& node) {
        //std::cout << box.top << std::endl;
        if (root.Intersect(box)) {
            numInserted++;
        }
        objects.push_back(node);
    }
};

struct RenderSystem : System<Position, const Renderable, Mesh>,
                      Dependencies<QuadTreeSystem> {

    void Initialize(QuadTreeSystem& treeSystem) {
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

using AllComponents = ComponentTypes<Position, Renderable, Mesh, BoundingBox, Velocity, QuadTreeNode>;
using AllSystems = SystemTypes<RenderSystem, BoundingBoxSystem, VelocitySystem, QuadTreeSystem>;

using RegistryType = Registry<AllComponents>;
using SceneType = Scene<RegistryType, AllSystems>;

int main() {
    DebugTemplate<AllSystems::UniqueSystems>();

    RegistryType registry;
    SceneType scene(registry);
    
    for(int i=0; i<100000; i++) {
        auto object = scene.CreateGameObject();
        scene.AddComponent<Position>(object, 0.0f, 0.0f);
        scene.AddComponent<Mesh>(object);
        auto& mesh = scene.GetComponent<Mesh>(object);
        mesh.vertices = { {0,0}, {10,0}, {10,10}, {0,10}};
        mesh.indicies = {0,1,2,0,2,3};
        scene.AddComponent<BoundingBox>(object);
        scene.AddComponent<QuadTreeNode>(object);
    }
    
    {
        Timer timer;
        scene.Update();
        std::cout << timer.Stop() << std::endl;
    }
    {
        Timer timer;
        scene.Update();
        std::cout << timer.Stop() << std::endl;
    }
    
    
    return EXIT_SUCCESS;
}
