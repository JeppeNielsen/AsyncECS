//
//  Screen.hpp
//  MiniOSX
//
//  Created by Jeppe Nielsen on 08/01/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <glm/vec2.hpp>
#include "Property.hpp"

namespace Game {
    class Screen {
    public:
        Property<glm::ivec2> Size;
        Property<float> ScalingFactor;
        
        static Screen* MainScreen;
    };
}
