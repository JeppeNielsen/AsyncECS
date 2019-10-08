//
//  GameObject.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//
#pragma once
#include <cstdint>

namespace AsyncECS {

using GameObject = std::uint32_t;
static constexpr auto GameObjectIndexMask = 0xFFFFF;
static constexpr auto GameObjectNull = ~0;

}


