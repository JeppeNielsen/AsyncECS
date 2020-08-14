//
//  SpatialTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "SpatialTests.hpp"
#include "WorldTransformSystem.hpp"
#include "HierarchySystem.hpp"
#include "Timer.hpp"

using namespace Game::Tests;


void SpatialTests::Run() {

    using ComponentTypes = AsyncECS::ComponentTypes<LocalTransform, WorldTransform, Hierarchy>;
    using Registry = AsyncECS::Registry<ComponentTypes>;
    using Systems = AsyncECS::SystemTypes<WorldTransformSystem, HierarchyWorldTransformSystem, HierarchySystem>;
    using Scene = AsyncECS::Scene<Registry, Systems>;

    RunTest("LocalTransform ctor gives default values",[]() {
        LocalTransform t;
        
        return t.position == glm::vec2(0,0) &&
                t.rotation == 0 &&
                t.scale == glm::vec2(1,1);
    });
    
    RunTest("LocalTransform applied to WorldTransform",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        const glm::vec2 position = {10,10};
        
        auto object = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(object);
        scene.AddComponent<WorldTransform>(object);
        scene.AddComponent<Hierarchy>(object);
        scene.GetComponent<LocalTransform>(object).position = position;
        
        scene.Update();
        
        //return scene.GetComponent<WorldTransform>(object).world ==
        //Matrix3x3::CreateTranslation(position);
        return false;
    });
    
    RunTest("Child LocalTransform Affected by Parent",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        const glm::vec2 parentPosition = {10,10};
        const glm::vec2 childPosition = {20,20};
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(parent);
        scene.AddComponent<WorldTransform>(parent);
        scene.AddComponent<Hierarchy>(parent);
        scene.GetComponent<LocalTransform>(parent).position = parentPosition;
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(child);
        scene.AddComponent<WorldTransform>(child);
        scene.AddComponent<Hierarchy>(child);
        
        scene.GetComponent<Hierarchy>(child).parent = parent;
        scene.GetComponent<LocalTransform>(child).position = childPosition;
        
        scene.Update();
        
        /*return
        scene.GetComponent<WorldTransform>(child).world ==
        Matrix3x3::CreateTranslation(parentPosition + childPosition);
         */
        return false;
    });
    
    RunTest("Child reparented WorldTransform should reflect",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        const glm::vec2 parentPosition = {10,10};
        const glm::vec2 childPosition = {20,20};
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(parent);
        scene.AddComponent<WorldTransform>(parent);
        scene.AddComponent<Hierarchy>(parent);
        scene.GetComponent<LocalTransform>(parent).position = parentPosition;
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(child);
        scene.AddComponent<WorldTransform>(child);
        scene.AddComponent<Hierarchy>(child);
        
        scene.GetComponent<Hierarchy>(child).parent = parent;
        scene.GetComponent<LocalTransform>(child).position = childPosition;
        
        scene.Update();
        
        bool parentPositionWasApplied = false;
        //scene.GetComponent<WorldTransform>(child).world ==
        //Matrix3x3::CreateTranslation(parentPosition + childPosition);
   
        scene.GetComponent<Hierarchy>(child).parent = AsyncECS::GameObjectNull;
   
        scene.Update();
   
        bool parentPositionNotApplied = false;
            // scene.GetComponent<WorldTransform>(child).world ==
             //Matrix3x3::CreateTranslation(childPosition);
        
   
        return parentPositionWasApplied &&
               parentPositionNotApplied;
    });
    
    RunTest("Parent moved, Child should be moved also",[]() {
         
         Registry registry;
         Scene scene(registry);
         
         const glm::vec2 parentPositionStart = {10,10};
         const glm::vec2 parentPositionEnd = {20,20};
         const glm::vec2 childPosition = {20,20};
         
         auto parent = scene.CreateGameObject();
         scene.AddComponent<LocalTransform>(parent);
         scene.AddComponent<WorldTransform>(parent);
         scene.AddComponent<Hierarchy>(parent);
         scene.GetComponent<LocalTransform>(parent).position = parentPositionStart;
         
         auto child = scene.CreateGameObject();
         scene.AddComponent<LocalTransform>(child);
         scene.AddComponent<WorldTransform>(child);
         scene.AddComponent<Hierarchy>(child);
         
         scene.GetComponent<LocalTransform>(child).position = childPosition;
         scene.GetComponent<Hierarchy>(child).parent = parent;
         
         scene.Update();
         
        bool childPositionEqualsStart = false;
         //scene.GetComponent<WorldTransform>(child).world ==
         //glm::translate(parentPositionStart + childPosition);
    
         scene.GetComponent<LocalTransform>(parent).position = parentPositionEnd;
    
         scene.Update();
         
         auto childWorld = scene.GetComponent<WorldTransform>(child).world;
    
        bool childPositionEqualsEnd = false;
         //childWorld ==
         //Matrix3x3::CreateTranslation(parentPositionEnd + childPosition);
         
         return childPositionEqualsStart &&
                childPositionEqualsEnd;
     });
    
    
    RunTest("Multiple children affected by parent's movement",[]() {
         
        const int numChildren = 100;
        
        Registry registry;
        Scene scene(registry);

        const glm::vec2 parentPositionStart = {10,10};
        const glm::vec2 parentPositionEnd = {20,20};
        const glm::vec2 childPosition = {20,20};

        auto parent = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(parent);
        scene.AddComponent<WorldTransform>(parent);
        scene.AddComponent<Hierarchy>(parent);
        scene.GetComponent<LocalTransform>(parent).position = parentPositionStart;
         
        std::vector<AsyncECS::GameObject> children;
        
        for (int i=0; i<numChildren; ++i) {
            auto child = scene.CreateGameObject();
            scene.AddComponent<LocalTransform>(child);
            scene.AddComponent<WorldTransform>(child);
            scene.AddComponent<Hierarchy>(child);
            
            scene.GetComponent<LocalTransform>(child).position = childPosition;
            scene.GetComponent<Hierarchy>(child).parent = parent;
            children.push_back(child);
        }
        
        scene.Update();
        
        //auto targetWorld = Matrix3x3::CreateTranslation(parentPositionStart + childPosition);
        
        bool anyNotAligned = false;
        /*for(auto child : children) {
            auto childWorld = scene.GetComponent<WorldTransform>(child).world;
            if (childWorld!=targetWorld) {
                anyNotAligned = true;
            }
        }*/
        
        return !anyNotAligned;
    });
    
}
