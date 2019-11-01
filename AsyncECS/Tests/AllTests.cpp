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

using namespace Tests;

void AllTests::Run() {

    {
        std::cout << "ComponentContainer : " << std::endl;
        TestComponentContainer test;
        test.Run();
    }


}
