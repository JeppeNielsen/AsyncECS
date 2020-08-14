//
//  SystemTask.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/11/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Timer.hpp"

namespace AsyncECS {
    struct SystemTask {
        
        void Precede(SystemTask& task);
        
        using Links = std::vector<SystemTask*>;
        Links incoming;
        Links outgoing;
        std::string name;
        std::function<void()> work;
        
        bool isDone = false;
        
        bool IsReady();
        
        float lastTime;
        
    };
}
