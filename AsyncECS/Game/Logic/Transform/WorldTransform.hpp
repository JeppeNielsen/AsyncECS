//
//  WorldTransform.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Matrix3x3.hpp"

namespace Game {
    struct WorldTransform {
        Matrix3x3 world;
        Matrix3x3 worldInverse;
        bool isDirty = true;
    };
}
