//
//  LogicTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "LogicTests.hpp"
#include "SpatialTests.hpp"
#include "HierarchicalTests.hpp"
#include <iostream>

using namespace Game::Tests;

void LogicTests::Run() {
    HierarchicalTests hierarchicalTests;
    hierarchicalTests.Run();
    std::cout << std::endl;
    SpatialTests spatialTests;
    spatialTests.Run();
}
