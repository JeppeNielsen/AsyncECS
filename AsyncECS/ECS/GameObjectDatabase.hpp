//
//  GameObjectDatabase.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <vector>
#include "GameObject.hpp"

namespace AsyncECS {
    class GameObjectDatabase {
    public:
        bool IsValid(const GameObject object) const;
        GameObject Create();
        void Remove(const GameObject objectId);
    private:
        std::vector<GameObject> objects;
        std::size_t available {};
        GameObject next {};
    };
}
