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
    
        void RunTask(std::function<void()> work, std::function<void()> finished = nullptr);
        bool Update();
        
        struct Task {
            std::function<void()> work;
            std::function<void()> finished;
            std::future<void> future;
           
            Task(std::function<void()> finished, std::function<void()> work) : finished(finished) {
                future = std::async(std::launch::async, work);
            }
            
            bool HasFinished() {
                return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
            }
        };
        
        using Tasks = std::vector<std::unique_ptr<Task>>;
        
        Tasks tasks;
    };
}

