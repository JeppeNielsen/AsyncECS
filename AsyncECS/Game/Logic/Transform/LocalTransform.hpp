//
//  Transform.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Vector2.hpp"
#include "Matrix3x3.hpp"

namespace Game {
    struct LocalTransform {
        LocalTransform();
        Vector2 position;
        float rotation;
        Vector2 scale;
    };
}
