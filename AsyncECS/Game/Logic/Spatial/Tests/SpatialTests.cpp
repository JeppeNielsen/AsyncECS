//
//  SpatialTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "SpatialTests.hpp"
#include "WorldTransformSystem.hpp"
#include "AssignChildrenSystem.hpp"

using namespace Game::Tests;


void SpatialTests::Run() {

    using ComponentTypes = AsyncECS::ComponentTypes<LocalTransform, WorldTransform, Parent, Children>;
    using Registry = AsyncECS::Registry<ComponentTypes>;
    using Systems = AsyncECS::SystemTypes<WorldTransformSystem, ChildrenWorldTransformSystem, AssignChildrenSystem>;

    RunTest("LocalTransform ctor gives default values",[]() {
        LocalTransform t;
        
        return t.position == Vector2(0,0) &&
                t.rotation == 0 &&
                t.scale == Vector2(1,1);
    });
    
    RunTest("LocalTransform applied to WorldTransform",[]() {
        
        Registry registry;
        AsyncECS::Scene<Registry, Systems> scene(registry);
        
        const Vector2 position = {10,10};
        
        auto object = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(object);
        scene.AddComponent<WorldTransform>(object);
        scene.AddComponent<Parent>(object);
        scene.GetComponent<LocalTransform>(object).position = position;
        
        scene.Update();
        
        return scene.GetComponent<WorldTransform>(object).world ==
        Matrix3x3::CreateTranslation(position);
    });
    
    RunTest("Child LocalTransform Affected by Parent",[]() {
        
        Registry registry;
        AsyncECS::Scene<Registry, Systems> scene(registry);
        
        const Vector2 parentPosition = {10,10};
        const Vector2 childPosition = {20,20};
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(parent);
        scene.AddComponent<WorldTransform>(parent);
        scene.AddComponent<Parent>(parent);
        scene.GetComponent<LocalTransform>(parent).position = parentPosition;
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(child);
        scene.AddComponent<WorldTransform>(child);
        scene.AddComponent<Parent>(child);
        
        scene.GetComponent<Parent>(child).parent = parent;
        scene.GetComponent<LocalTransform>(child).position = childPosition;
        
        scene.Update();
        
        return
        scene.GetComponent<WorldTransform>(child).world ==
        Matrix3x3::CreateTranslation(parentPosition + childPosition);
    });
    
    RunTest("Child reparented WorldTransform should reflect",[]() {
        
        Registry registry;
        AsyncECS::Scene<Registry, Systems> scene(registry);
        
        const Vector2 parentPosition = {10,10};
        const Vector2 childPosition = {20,20};
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(parent);
        scene.AddComponent<WorldTransform>(parent);
        scene.AddComponent<Parent>(parent);
        scene.GetComponent<LocalTransform>(parent).position = parentPosition;
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<LocalTransform>(child);
        scene.AddComponent<WorldTransform>(child);
        scene.AddComponent<Parent>(child);
        
        scene.GetComponent<Parent>(child).parent = parent;
        scene.GetComponent<LocalTransform>(child).position = childPosition;
        
        scene.Update();
        
        bool parentPositionWasApplied =
        scene.GetComponent<WorldTransform>(child).world ==
        Matrix3x3::CreateTranslation(parentPosition + childPosition);
   
        scene.GetComponent<Parent>(child).parent = GameObjectNull;
   
        scene.Update();
   
        bool parentPositionNotApplied =
             scene.GetComponent<WorldTransform>(child).world ==
             Matrix3x3::CreateTranslation(childPosition);
        
   
        return parentPositionWasApplied &&
               parentPositionNotApplied;
    });
    
    RunTest("Parent moved, Child should be moved also",[]() {
         
         Registry registry;
         AsyncECS::Scene<Registry, Systems> scene(registry);
         
         const Vector2 parentPositionStart = {10,10};
         const Vector2 parentPositionEnd = {20,20};
         const Vector2 childPosition = {20,20};
         
         auto parent = scene.CreateGameObject();
         scene.AddComponent<LocalTransform>(parent);
         scene.AddComponent<WorldTransform>(parent);
         scene.AddComponent<Parent>(parent);
         scene.AddComponent<Children>(parent);
         scene.GetComponent<LocalTransform>(parent).position = parentPositionStart;
         
         auto child = scene.CreateGameObject();
         scene.AddComponent<LocalTransform>(child);
         scene.AddComponent<WorldTransform>(child);
         scene.AddComponent<Parent>(child);
         
         scene.GetComponent<Parent>(child).parent = parent;
         scene.GetComponent<LocalTransform>(child).position = childPosition;
         
         scene.Update();
         
         bool childPositionEqualsStart =
         scene.GetComponent<WorldTransform>(child).world ==
         Matrix3x3::CreateTranslation(parentPositionStart + childPosition);
    
         scene.GetComponent<LocalTransform>(parent).position = parentPositionEnd;
    
         scene.Update();
         
         auto childWorld = scene.GetComponent<WorldTransform>(child).world;
    
         bool childPositionEqualsEnd =
         childWorld ==
         Matrix3x3::CreateTranslation(parentPositionEnd + childPosition);
         
         return childPositionEqualsStart &&
                childPositionEqualsEnd;
     });
    
}
