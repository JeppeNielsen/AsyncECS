//
//  Timer.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/09/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <chrono>
#include <string>

class Timer {
    std::chrono::steady_clock::time_point startTime;
public:
    Timer();
    int Stop();
};
