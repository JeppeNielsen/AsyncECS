//
//  Hierarchy.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 25/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "GameObject.hpp"
#include <vector>

namespace Game {
    struct Hierarchy {
        AsyncECS::GameObject parent = AsyncECS::GameObjectNull;
        AsyncECS::GameObject previousParent = AsyncECS::GameObjectNull;
        std::vector<AsyncECS::GameObject> children;
    };
}
