//
//  Transform.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "LocalTransform.hpp"

using namespace Game;

LocalTransform::LocalTransform() : position({0.0f,0.0f,0.0f}), rotation(quat(1.0f,0.0f,0.0f,0.0f)), scale({1.0f,1.0f,1.0f}) {
    
}
