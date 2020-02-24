//
//  SpatialTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "HierarchicalTests.hpp"
#include "AssignChildrenSystem.hpp"
#include "Scene.hpp"

using namespace Game::Tests;

void HierarchicalTests::Run() {

    RunTest("Children ctor gives default values",[]() {
        Children c;
        
        return c.assignedParent == GameObjectNull &&
                c.children.size() == 0;
    });
    
    RunTest("Parent children contains child",[]() {
        
        using Components = AsyncECS::ComponentTypes<Parent, Children>;
        using Systems = AsyncECS::SystemTypes<AssignChildrenSystem>;
        using Registry = AsyncECS::Registry<Components>;
        using Scene = Scene<Registry, Systems>;
        Registry registry;
        Scene scene(registry);
        
        auto parent = scene.CreateGameObject();
        scene.AddComponent<Parent>(parent);
        scene.AddComponent<Children>(parent);
        
        auto child = scene.CreateGameObject();
        scene.AddComponent<Parent>(child);
        scene.AddComponent<Children>(child);
        
        scene.GetComponent<Parent>(child).parent = parent;
        
        scene.Update();
        
        auto& childrenInParent = scene.GetComponent<Children>(parent).children;
        
        
        return std::find(childrenInParent.begin(), childrenInParent.end(), child) != childrenInParent.end();
    });
    
    
}
