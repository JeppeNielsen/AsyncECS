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
            std::atomic<bool> isFinished;
           
            Task(std::function<void()> finished, std::function<void()> work) : finished(std::move(finished)), work(std::move(work)) {
                isFinished = false;
                future = std::async(std::launch::async, [this]() {
                    this->work();
                    isFinished = true;
                });
            }
            
            bool HasFinished() {
                //future.wait();
                return isFinished;
                
                //return true;
            //    return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
            }
        };
        
        using Tasks = std::vector<std::unique_ptr<Task>>;
        
        Tasks tasks;
    };
}

