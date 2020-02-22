//
//  RunLogicTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "TestTaskRunner.hpp"
#include "LogicTests.hpp"

int main() {
    Tests::TestTaskRunner taskRunner;
    taskRunner.Run();


for (int i=0; i<1; i++) {
    Game::Tests::LogicTests logicTests;
    logicTests.Run();
}
    
}
