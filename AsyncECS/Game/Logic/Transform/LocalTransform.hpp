//
//  Transform.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <glm/vec2.hpp>

namespace Game {
    struct LocalTransform {
        LocalTransform();
        glm::vec2 position;
        float rotation;
        glm::vec2 scale;
    };
}
