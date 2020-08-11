//
//  RunLogicTests.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include <iostream>
#include "AllTests.hpp"
#include "TestTaskRunner.hpp"
#include "LogicTests.hpp"

int main_tests() {
    AllTests allTests;
    allTests.Run();
    std::cout << "\nLogic Tests: \n";
    Game::Tests::LogicTests logicTests;
    logicTests.Run();
    return 0;
}
    
