//
//  TestGameObjectDatabase.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 01/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include "TestGameObjectDatabase.hpp"
#include "GameObjectDatabase.hpp"

using namespace Tests;
using namespace AsyncECS;

void TestGameObjectDatabase::Run() {

    RunTest("Create, IsValid", [] () {
        GameObjectDatabase database;
        auto object = database.Create();
        return database.IsValid(object);
    });

    RunTest("Create, size == 1", [] () {
        GameObjectDatabase database;
        database.Create();
        return database.Size() == 1;
    });

    RunTest("Remove", [] () {
        GameObjectDatabase database;
        auto object = database.Create();
        bool wasOne = database.Size() == 1;
        database.Remove(object);
        return wasOne && !database.IsValid(object);
    });
    
    RunTest("Default GameObject not valid", [] () {
        GameObjectDatabase database;
        GameObject object = 0;
        return !database.IsValid(object);
    });

    RunTest("Add remove different ids", [] () {
        GameObjectDatabase database;
        auto object1 = database.Create();
        database.Remove(object1);
        auto object2 = database.Create();
        return object1!=object2 && database.Size() == 1;
    });
}
