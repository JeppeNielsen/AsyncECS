//
//  SpatialTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "SpatialTests.hpp"
#include "LocalTransformSystem.hpp"
#include "ParentTransformSystem.hpp"
#include "TransformSystem.hpp"
#include "WorldTransformSystem.hpp"

using namespace Game::Tests;

struct ExpensiveComponent {
    int waitTime;
};

struct ExpensionSystem : System<ExpensiveComponent>, NoSystemDependencies, NoComponentView {
    void Initialize() {
    
    }

    void Update(ExpensiveComponent& component) {
        while (component.waitTime>0) {
            component.waitTime--;
            float d = 0;
            for (int i=0; i<100000; i++) {
                d += sin(i*10)*0.01f;
            }
        }
    }
};

void SpatialTests::Run() {


/*
    RunTest("Expensive system",[]() {
        
        using ComponentTypes = AsyncECS::ComponentTypes<ExpensiveComponent>;
        using Systems = AsyncECS::SystemTypes<ExpensionSystem>;
        using Registry = AsyncECS::Registry<ComponentTypes>;
        
        Registry registry;
        using Scene = AsyncECS::Scene<Registry, Systems>;
        Scene scene(registry);
        
        auto go = scene.CreateGameObject();
        scene.AddComponent<ExpensiveComponent>(go, 100);
        
        scene.Update();
        
        return true;
    });
    
*/

    RunTest("LocalTransform ctor gives default values",[]() {
        LocalTransform t;
        
        return t.position == Vector2(0,0) &&
                t.rotation == 0 &&
                t.scale == Vector2(1,1);
    });

    RunTest("TransformSystem Calculate WorldTransform",[]() {
        
        using ComponentTypes = AsyncECS::ComponentTypes<LocalTransform, WorldTransform, Parent, DirtyTransform>;
        using Registry = AsyncECS::Registry<ComponentTypes>;
        using Systems = AsyncECS::SystemTypes<LocalTransformSystem, ParentTransformSystem, TransformSystem>;
        
        Registry registry;
        AsyncECS::Scene<Registry, Systems> scene(registry);
        
        auto go = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(go);
        scene.AddComponent<WorldTransform>(go);
        scene.AddComponent<Parent>(go);
        scene.AddComponent<DirtyTransform>(go);
        
        auto& localTransform = scene.GetComponent<LocalTransform>(go);
        localTransform.position = {12, 34};
        auto& worldTransform = scene.GetComponent<WorldTransform>(go);
        
        scene.Update();
        scene.WriteGraph(std::cout);
        
        Matrix3x3 target = Matrix3x3::CreateTranslation({12, 34});
        return worldTransform.world == target;
    });
 
    
    RunTest("Parent/Child WorldTransform",[]() {
        
        using ComponentTypes = AsyncECS::ComponentTypes<LocalTransform, WorldTransform, Parent, DirtyTransform>;
        using Registry = AsyncECS::Registry<ComponentTypes>;
        using Systems = AsyncECS::SystemTypes<LocalTransformSystem, ParentTransformSystem, TransformSystem>;
        
        Registry registry;
        AsyncECS::Scene<Registry, Systems> scene(registry);
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(parent);
        scene.AddComponent<WorldTransform>(parent);
        scene.AddComponent<Parent>(parent);
        scene.AddComponent<DirtyTransform>(parent);
        scene.GetComponent<LocalTransform>(parent).position = {10,10};
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(child);
        scene.AddComponent<WorldTransform>(child);
        scene.AddComponent<Parent>(child);
        scene.AddComponent<DirtyTransform>(child);
        
       
        
        scene.GetComponent<Parent>(child).parent = parent;
        scene.GetComponent<LocalTransform>(child).position = {10,10};
        
        
        scene.Update();
        auto worldTransform1 = scene.GetComponent<WorldTransform>(child);
        Matrix3x3 target1 = Matrix3x3::CreateTranslation({20, 20});
        scene.GetComponent<Parent>(child).parent = GameObjectNull;
        scene.Update();
        auto worldTransform2 = scene.GetComponent<WorldTransform>(parent);
        Matrix3x3 target2 = Matrix3x3::CreateTranslation({10, 10});
        
        return worldTransform1.world == target1 && worldTransform2.world == target2;
    });
    
    RunTest("Parent/Child WorldTransform no transform",[]() {
        
        using ComponentTypes = AsyncECS::ComponentTypes<LocalTransform, WorldTransform, Parent>;
        using Registry = AsyncECS::Registry<ComponentTypes>;
        using Systems = AsyncECS::SystemTypes<WorldTransformSystem>;
        
        Registry registry;
        AsyncECS::Scene<Registry, Systems> scene(registry);
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(parent);
        scene.AddComponent<WorldTransform>(parent);
        scene.AddComponent<Parent>(parent);
        scene.GetComponent<LocalTransform>(parent).position = {10,10};
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(child);
        scene.AddComponent<WorldTransform>(child);
        scene.AddComponent<Parent>(child);
        
        scene.GetComponent<Parent>(child).parent = parent;
        scene.GetComponent<LocalTransform>(child).position = {10,10};
        
        
        scene.Update();
        auto worldTransform1 = scene.GetComponent<WorldTransform>(child);
        Matrix3x3 target1 = Matrix3x3::CreateTranslation({20, 20});
        scene.GetComponent<Parent>(child).parent = GameObjectNull;
        scene.Update();
        auto worldTransform2 = scene.GetComponent<WorldTransform>(child);
        Matrix3x3 target2 = Matrix3x3::CreateTranslation({10, 10});
        scene.GetComponent<Parent>(child).parent = parent;
        scene.Update();
        auto worldTransform3 = scene.GetComponent<WorldTransform>(child);
        Matrix3x3 target3 = Matrix3x3::CreateTranslation({20, 20});
        
        scene.WriteGraph(std::cout);
        
        return
        worldTransform1.world == target1 &&
        worldTransform2.world == target2 &&
        worldTransform3.world == target3;
    });
    
}
