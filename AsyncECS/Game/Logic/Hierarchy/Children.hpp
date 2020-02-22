//
//  Children.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 20/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <vector>
#include "GameObject.hpp"

namespace Game {
  struct Children {
    std::vector<AsyncECS::GameObject> children;
  };
}
