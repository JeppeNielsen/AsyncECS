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
    tasks.emplace_back(std::make_unique<Task>( finished, work ));
}

bool TaskRunner::Update() {
    for(int i=0; i<tasks.size(); ++i) {
        auto& task = tasks[i];
        if (task->HasFinished()) {
            task->finished();
            tasks.erase(tasks.begin() + i);
            i--;
        }
    }
    return !tasks.empty();
}
