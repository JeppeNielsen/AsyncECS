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

void HierarchicalTests::Run() {

    RunTest("Hierarchy ctor gives default values",[]() {
        Hierarchy h;
        
        return h.previousParent == AsyncECS::GameObjectNull &&
                h.parent == AsyncECS::GameObjectNull &&
                h.children.size() == 0;
    });
    
    RunTest("Parent children contains child",[]() {
        
        using Components = AsyncECS::ComponentTypes<Hierarchy>;
        using Systems = AsyncECS::SystemTypes<HierarchySystem>;
        using Registry = AsyncECS::Registry<Components>;
        using Scene = AsyncECS::Scene<Registry, Systems>;
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
    
}
