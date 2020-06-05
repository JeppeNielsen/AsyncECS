//
//  SpatialTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "HierarchicalTests.hpp"
#include "HierarchySystem.hpp"
#include "Scene.hpp"

using namespace Game::Tests;
using namespace AsyncECS;

void HierarchicalTests::Run() {

    using Components = ComponentTypes<Hierarchy>;
    using Systems = SystemTypes<HierarchySystem>;
    using Registry = Registry<Components>;
    using Scene = Scene<Registry, Systems>;

    RunTest("Hierarchy ctor gives default values",[]() {
        Hierarchy h;
        
        return h.previousParent == GameObjectNull &&
                h.parent == GameObjectNull &&
                h.children.size() == 0;
    });
    
    RunTest("Parent children contains child",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(parent);
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(child);
        
        scene.GetComponent<Hierarchy>(child).parent = parent;
        
        scene.Update();
        
        auto& childrenInParent = scene.GetComponent<Hierarchy>(parent).children;
        
        return std::find(childrenInParent.begin(), childrenInParent.end(), child) != childrenInParent.end();
    });
    
    RunTest("Child removed from parent's children list on reparent",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(parent);
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(child);
        
        scene.GetComponent<Hierarchy>(child).parent = parent;
        
        scene.Update();
        
        auto& childrenInParent = scene.GetComponent<Hierarchy>(parent).children;
        bool wasInParent = std::find(childrenInParent.begin(), childrenInParent.end(), child) != childrenInParent.end();
        
        scene.GetComponent<Hierarchy>(child).parent = GameObjectNull;
        
        scene.Update();
        
        return wasInParent &&
                std::find(childrenInParent.begin(), childrenInParent.end(), child) == childrenInParent.end();
        
    });
    
    RunTest("Child removed from parent's children list on removal",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(parent);
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(child);
        
        scene.GetComponent<Hierarchy>(child).parent = parent;
        
        scene.Update();
        
        auto& childrenInParent = scene.GetComponent<Hierarchy>(parent).children;
        bool wasInParent = std::find(childrenInParent.begin(), childrenInParent.end(), child) != childrenInParent.end();
        
        scene.RemoveGameObject(child);
    
        scene.Update();
        
        return wasInParent &&
                std::find(childrenInParent.begin(), childrenInParent.end(), child) == childrenInParent.end();
    });
    
    RunTest("Child removed from parent's children list on removal, even when parent is removed also",[]() {
        
        Registry registry;
        Scene scene(registry);
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(parent);
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(child);
        
        scene.GetComponent<Hierarchy>(child).parent = parent;
        
        scene.Update();
        
        auto& childrenInParent = scene.GetComponent<Hierarchy>(parent).children;
        bool wasInParent = std::find(childrenInParent.begin(), childrenInParent.end(), child) != childrenInParent.end();
        
        scene.RemoveGameObject(child);
        scene.RemoveGameObject(parent);
        
        scene.Update();
        
        return wasInParent &&
                std::find(childrenInParent.begin(), childrenInParent.end(), child) == childrenInParent.end();
    });
    
    RunTest("Parent removed will also remove child",[]() {
        Registry registry;
        Scene scene(registry);

        auto parent = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(parent);

        auto child = scene.CreateGameObject();
        scene.AddComponent<Hierarchy>(child);

        scene.GetComponent<Hierarchy>(child).parent = parent;

        scene.Update();

        scene.RemoveGameObject(parent);

        bool isParentValid = registry.IsGameObjectValid(parent);
        bool isChildValid = registry.IsGameObjectValid(child);

        return
        !isParentValid &&
        !isChildValid;
    });
    
}
