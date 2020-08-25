//
//  InputSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 21/08/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "InputDevice.hpp"
#include "System.hpp"
#include "Input.hpp"

namespace Game {
    struct InputSystem : AsyncECS::System<Input> {
        
        void Update(Input& input);
        
        void SetDevice(const InputDevice& inputDevice);
    
        const InputDevice* inputDevice;
    };
}
