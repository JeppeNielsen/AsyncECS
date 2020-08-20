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
#include "BoundingBoxTests.hpp"
#include "OctreeSystemTests.hpp"
#include "RenderSystemTests.hpp"
#include <iostream>

using namespace Game::Tests;

void LogicTests::Run() {
    HierarchicalTests hierarchicalTests;
    hierarchicalTests.Run();
    std::cout << "\n";
    SpatialTests spatialTests;
    spatialTests.Run();
    std::cout << "\n";
    BoundingBoxTests boundingBoxTests;
    boundingBoxTests.Run();
    std::cout << "\n";
    OctreeSystemTests octreeSystemTests;
    octreeSystemTests.Run();
    std::cout << "\n";
    RenderSystemTests renderSystemTests;
    renderSystemTests.Run();
}
