//
//  TaskRunner.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 23/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <future>
#include <vector>
#include <functional>

namespace AsyncECS {
    struct TaskRunner {
    
        void RunTask(std::function<void()> work, std::function<void()> finished);
        bool Update();
        
        struct Task {
            std::function<void()> work;
            std::function<void()> finished;
            std::atomic<bool> isFinished;
        };
        
        using Tasks = std::vector<std::unique_ptr<Task>>;
        
        Tasks tasks;
    };
}

