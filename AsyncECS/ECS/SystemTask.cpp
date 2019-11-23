//
//  SystemTask.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include "SystemTask.hpp"

using namespace AsyncECS;

void SystemTask::Precede(SystemTask &task) {
    outgoing.push_back(&task);
    task.incoming.push_back(this);
}

bool SystemTask::IsReady() {
    for(auto incomingTask : incoming) {
        if (!incomingTask->isDone) return false;
    }
    return true;
}
