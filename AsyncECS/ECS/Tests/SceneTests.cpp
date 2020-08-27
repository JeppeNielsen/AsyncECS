//
//  SceneTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 27/08/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "SceneTests.hpp"
#include "Scene.hpp"

using namespace AsyncECS;
using namespace AsyncECS::Tests;

struct Point {
    float x;
    float y;
};

struct Mesh {
    std::vector<Point> points;
    std::vector<int> indicies;
};

struct MeshSystem : System<Mesh> {
  
    void Update(Mesh& mesh) {
        
    }
    
};

void SceneTests::Run() {

    using Components = ComponentTypes<Mesh>;
    using Systems = SystemTypes<MeshSystem>;
    using Registry = Registry<Components>;
    using Scene = Scene<Registry, Systems>;
    
    RunTest("Removed GameObject should not be valid",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        auto go = scene.CreateGameObject();
        scene.Update();
        scene.RemoveGameObject(go);
        scene.Update();
        
        return !registry.IsGameObjectValid(go);
    });
    
    RunTest("Recreated game object should be one version higher",[]() {
        Registry registry;
        Scene scene(registry);

        auto firstGo = scene.CreateGameObject();
        int firstVersion = registry.GetGameObjectVersion(firstGo);

        scene.Update();
        scene.RemoveGameObject(firstGo);
        scene.Update();
        auto secondGo = scene.CreateGameObject();
        scene.Update();
        int secondVersion = registry.GetGameObjectVersion(secondGo);
        scene.RemoveGameObject(secondGo);
        scene.Update();
        auto thirdGo = scene.CreateGameObject();
        scene.Update();
        auto thirdVersion = registry.GetGameObjectVersion(thirdGo);
        
        return (firstVersion + 1) == secondVersion &&
               (secondVersion + 1) == thirdVersion;
    });

    RunTest("Remove GameObject should reset component",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        auto go = scene.CreateGameObject();
        scene.AddComponent<Mesh>(go);
        
        auto& mesh = scene.GetComponent<Mesh>(go);
        mesh.points.resize(5);
        mesh.indicies.resize(10);
        
        scene.Update();
        scene.RemoveGameObject(go);
        scene.Update();
        
        auto newGo = scene.CreateGameObject();
        scene.AddComponent<Mesh>(newGo);
        auto& newMesh = scene.GetComponent<Mesh>(newGo);
        
        return newMesh.indicies.empty() && newMesh.points.empty();
    });
}
