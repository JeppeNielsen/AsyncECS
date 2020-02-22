//
//  Hierarchy.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 19/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "GameObject.hpp"

using namespace AsyncECS;

namespace Game {
    struct Parent {
        GameObject parent = GameObjectNull;
    };
}
