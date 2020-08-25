//
//  InputSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 21/08/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "InputSystem.hpp"


using namespace Game;

void InputSystem::SetDevice(const InputDevice& inputDevice) {
    this->inputDevice = &inputDevice;
}

void InputSystem::Update(Input &input) {
    const InputDevice& d = *inputDevice;
    
    input.touchesDown.clear();
    input.touchesUp.clear();
    
    for (int i=0; i<d.currentTouches.size(); ++i) {
        const auto& currentTouch = d.currentTouches[i];
        const auto& previousTouch = d.previousTouches[i];
        input.touchPosition[i] =  TouchPosition { currentTouch.Position };
        
        if (currentTouch.IsDown == previousTouch.IsDown) {
            continue;
        }
        
        InputTouch t;
        t.index = i;
        
        if (currentTouch.IsDown) {
            input.touchesDown.emplace_back(t);
        } else {
            input.touchesUp.emplace_back(t);
        }
    }
    
}
