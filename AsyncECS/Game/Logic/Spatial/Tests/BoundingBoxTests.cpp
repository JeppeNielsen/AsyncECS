//
//  BoundingBoxTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "BoundingBoxTests.hpp"
#include "ECS.hpp"
#include "WorldTransform.hpp"
#include "LocalBoundingBox.hpp"
#include "WorldBoundingBox.hpp"
#include "WorldBoundingBoxSystem.hpp"
#include <math.h>

#define ToRadians(degrees) (0.01745329251994329576923690768489 * degrees)

using namespace Game::Tests;
using namespace Game;

void BoundingBoxTests::Run() {
 
    using ComponentTypes = AsyncECS::ComponentTypes<WorldTransform, LocalBoundingBox, WorldBoundingBox>;
    using Registry = AsyncECS::Registry<ComponentTypes>;
    using Systems = AsyncECS::SystemTypes<WorldBoundingBoxSystem>;
    using Scene = AsyncECS::Scene<Registry, Systems>;

    RunTest("Local to World bounds, scale", [] () {
        Registry registry;
        Scene scene(registry);
        
        auto go = scene.CreateGameObject();
        scene.AddComponent<WorldTransform>(go);
        scene.AddComponent<LocalBoundingBox>(go);
        scene.AddComponent<WorldBoundingBox>(go);
        
        scene.GetComponent<WorldTransform>(go).world = scale(mat4x4(1.0f), {2.0f,2.0f,2.0f});
        scene.GetComponent<LocalBoundingBox>(go).bounds = {{0,0,0}, {10,10,10}};
        
        scene.Update();
        
        const BoundingBox target = {{0,0,0},{20,20,20}};
        return scene.GetComponent<WorldBoundingBox>(go).bounds == target;
    });
    
    RunTest("Local to World bounds, rotation", [] () {
        Registry registry;
        Scene scene(registry);
        
        auto go = scene.CreateGameObject();
        scene.AddComponent<WorldTransform>(go);
        scene.AddComponent<LocalBoundingBox>(go);
        scene.AddComponent<WorldBoundingBox>(go);
        
        //scene.GetComponent<WorldTransform>(go).world = Matrix3x3::CreateRotation(ToRadians(45));
        //scene.GetComponent<LocalBoundingBox>(go).bounds = {{0,0}, {10,10}};
        
        scene.Update();
        
        const BoundingBox target = {{0,0,0},{sqrt(2.0f) * 10.0f, sqrt(2.0f) * 10.0f, sqrt(2.0f) * 10.0f}};
        
        auto bounds = scene.GetComponent<WorldBoundingBox>(go).bounds;
        
        return bounds == target;
    });
    
}
