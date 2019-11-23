//
//  TaskRunner.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 23/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include "TaskRunner.hpp"

using namespace AsyncECS;

void TaskRunner::RunTask(std::function<void ()> work, std::function<void ()> finished) {
    auto task = std::make_unique<Task>();
    
    task->work = work;
    task->finished = finished;
    task->isFinished = false;
    
    std::async(std::launch::async, [&task, work] () {
        work();
        task->isFinished = true;
    });
    tasks.push_back(std::move(task));
}

bool TaskRunner::Update() {
    for(int i=0; i<tasks.size(); ++i) {
        auto& task = tasks[i];
        if (task->isFinished) {
            task->finished();
            tasks.erase(tasks.begin() + i);
            i--;
        }
    }
    return !tasks.empty();
}
