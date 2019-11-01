//
//  EntityCollection.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <vector>
#include "GameObject.hpp"

namespace AsyncECS {

class GameObjectCollection {
public:
    using Objects = std::vector<GameObject>;
    using Indicies = std::vector<GameObject>;

    void Add(const GameObject object);
    void Remove(const GameObject object);
    bool Contains(const GameObject object) const;
    std::size_t Get(const GameObject object) const;
    void Clear();

    Indicies indicies;
    Objects objects;
};
    
}
