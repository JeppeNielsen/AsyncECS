//
//  TestTaskRunner.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 21/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "TestTaskRunner.hpp"
#include "GameObjectDatabase.hpp"
#include "TaskRunner.hpp"
#include <cmath>
#include <iostream>

using namespace Tests;
using namespace AsyncECS;


void TestTaskRunner::Run() {

    RunTest("TaskRunner. Wait for all", [] () {
    
        TaskRunner taskRunner;
        
        taskRunner.RunTask([] {
            float d = 0;
            for (int i=0; i<1000000; ++i) {
                d+=sin(i*10)*0.01f;
            }
            
            std::cout << "From task"<< std::endl;
        });
        
        while (taskRunner.Update()) {
            int hej = 0;
            hej++;
        }
        
        return true;
    });


}
