//
//  LogicTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "LogicTests.hpp"
#include "SpatialTests.hpp"

using namespace Game::Tests;

void LogicTests::Run() {
    SpatialTests spatialTests;
    spatialTests.Run();
}
