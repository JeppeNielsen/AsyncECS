//
//  Tests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 01/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//
#include <iostream>
#include "AllTests.hpp"
#include "TestComponentContainer.hpp"
#include "TestGameObjectDatabase.hpp"
#include "TestTaskRunner.hpp"

using namespace Tests;

void AllTests::Run() {

    {
        std::cout << "ComponentContainer : " << std::endl;
        TestComponentContainer test;
        test.Run();
    }
    {
        std::cout << "GameObjectDatabase : " << std::endl;
        TestGameObjectDatabase test;
        test.Run();
    }
    
    {
        std::cout << "TaskRunner : " << std::endl;
        TestTaskRunner test;
        test.Run();
    }

}
