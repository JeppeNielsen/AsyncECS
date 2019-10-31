//
//  Timer.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/09/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include "Timer.hpp"
#include <iostream>

Timer::Timer() {
    startTime = std::chrono::steady_clock::now();
}

int Timer::Stop() {
    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
//    std::cout << name <<" : " << duration << std::endl;
    return static_cast<int>(duration);
}
