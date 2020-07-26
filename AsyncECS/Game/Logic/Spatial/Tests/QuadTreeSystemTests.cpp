//
//  QuadTreeSystemTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "QuadTreeSystemTests.hpp"
#include "ECS.hpp"
#include "WorldBoundingBox.hpp"
#include "QuadTreeSystem.hpp"
#include <math.h>

#define ToRadians(degrees) (0.01745329251994329576923690768489 * degrees)

using namespace Game::Tests;
using namespace Game;

void QuadTreeSystemTests::Run() {
 
    using ComponentTypes = AsyncECS::ComponentTypes<WorldBoundingBox>;
    using Registry = AsyncECS::Registry<ComponentTypes>;
    using Systems = AsyncECS::SystemTypes<QuadTreeSystem>;
    using Scene = AsyncECS::Scene<Registry, Systems>;

    RunTest("Quad tree insert", [] () {
        Registry registry;
        Scene scene(registry);
        
        auto go = scene.CreateGameObject();
        scene.AddComponent<WorldBoundingBox>(go);
        scene.GetComponent<WorldBoundingBox>(go).bounds = {{0,0}, {10,10}};
        
        auto& system = scene.GetSystem<QuadTreeSystem>();
        
        scene.Update();
        
        return system.quadTree.nodes[0]->data == go;
    });
    
    RunTest("Quad tree removal", [] () {
        Registry registry;
        Scene scene(registry);
        
        auto go = scene.CreateGameObject();
        scene.AddComponent<WorldBoundingBox>(go);
        scene.GetComponent<WorldBoundingBox>(go).bounds = {{0,0}, {10,10}};
        
        scene.Update();
        
        scene.RemoveGameObject(go);
        
        auto& system = scene.GetSystem<QuadTreeSystem>();
        
        return system.quadTree.nodes.size() == 0;
    });
    
}

