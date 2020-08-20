//
//  WorldTransform.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <glm/mat3x3.hpp>

namespace Game {
    struct WorldTransform {
        glm::mat4x4 world;
        glm::mat4x4 worldInverse;
        bool isDirty = true;
    };
}
