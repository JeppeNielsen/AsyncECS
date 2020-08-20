//
//  Camera.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "Camera.hpp"


using namespace Game;

mat4x4 Camera::GetProjection() const {
   return ortho(-ViewSize.x, ViewSize.x, -ViewSize.y, ViewSize.y, Near, Far);
}
