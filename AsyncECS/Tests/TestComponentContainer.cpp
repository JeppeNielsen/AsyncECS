//
//  TestComponentContainer.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 01/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include "TestComponentContainer.hpp"
#include "ComponentContainer.hpp"

using namespace Tests;
using namespace AsyncECS;

void TestComponentContainer::Run() {

    struct Position {
        float x;
        float y;
    };

    RunTest("Create",[] () {
        ComponentContainer<Position> container;
        GameObject go = 0;
        container.Create(go);
        return container.elements.size() == 1;
    });

    RunTest("Destroy",[] () {
        ComponentContainer<Position> container;
        GameObject go = 0;
        container.Create(go);
        bool wasOne = container.elements.size() == 1;
        container.Destroy(go);
        return container.elements.empty() && wasOne;
    });

    RunTest("Reference",[] () {
        ComponentContainer<Position> container;
        GameObject go1 = 0;
        GameObject go2 = 1;
        container.Create(go1);
        container.Reference(go2, go1);
        return &container.Get(go1) == &container.Get(go2);
    });
    
    RunTest("Get, adds to changedThisFrame",[] () {
        ComponentContainer<Position> container;
        GameObject go1 = 0;
        container.Create(go1);
        container.Get(go1);
        return container.changedThisFrame.objects.size() == 1;
    });
    
    RunTest("Double get only adds one entry in changedThisFrame",[] () {
        ComponentContainer<Position> container;
        GameObject go1 = 0;
        container.Create(go1);
        container.Get(go1);
        container.Get(go1);
        return container.changedThisFrame.objects.size() == 1;
    });
    
    RunTest("GetConst doesn't add to changedThisFrame",[] () {
        ComponentContainer<Position> container;
        GameObject go1 = 0;
        container.Create(go1);
        container.GetConst(go1);
        return container.changedThisFrame.objects.size() == 0;
    });
    
    RunTest("Get also adds changedThisFrame for references",[] () {
        ComponentContainer<Position> container;
        GameObject go1 = 0;
        GameObject go2 = 1;
        
        container.Create(go1);
        container.Reference(go2, go1);
        
        container.Get(go1);
        
        return container.changedThisFrame.objects.size() == 2;
    });

    RunTest("Destroy referenced will not destroy element",[] () {
        ComponentContainer<Position> container;
        GameObject go1 = 0;
        GameObject go2 = 1;
        container.Create(go1);
        container.Reference(go2, go1);
        
        bool wasOne = container.elements.size() == 1;
        
        container.Destroy(go1);
        
        bool stillOne = container.elements.size() == 1;
        
        container.Destroy(go2);
        
        bool isNone = container.elements.empty();
        
        return wasOne && stillOne && isNone;
    });

    RunTest("Create variadic version",[] () {
        ComponentContainer<Position> container;
        GameObject go1 = 0;
        container.Create(go1, 1.0f, 2.0f);
        return container.Get(go1).x == 1.0f &&
               container.Get(go1).y == 2.0f;
    });
}
