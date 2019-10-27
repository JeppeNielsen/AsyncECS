//
//  EntityCollection.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include "GameObjectCollection.hpp"

using namespace AsyncECS;

void GameObjectCollection::Add(const GameObject object) {
    const auto objectIndex = object & GameObjectIndexMask;
    if (objectIndex>=indicies.size()) {
        indicies.resize(objectIndex + 1, GameObjectNull);
    }
    indicies[objectIndex] = (GameObject)objects.size();
    objects.emplace_back(object);
}

void GameObjectCollection::Remove(const GameObject object) {
    const auto objectIndex = object & GameObjectIndexMask;
    const auto last = objects.back();
    const auto lastIndex = last & GameObjectIndexMask;
    
    const auto indexInObjects = indicies[objectIndex];
    const auto lastIndexInObjects = indicies[lastIndex];
    
    indicies[lastIndex] = indexInObjects;
    indicies[objectIndex] = GameObjectNull;
    
    objects[indexInObjects] = objects[lastIndexInObjects];
    objects.pop_back();
}

bool GameObjectCollection::Contains(const GameObject object) const {
    const auto index = object & GameObjectIndexMask;
    return index<indicies.size() && indicies[index] != GameObjectNull;
}

std::size_t GameObjectCollection::Get(const GameObject object) const {
    return indicies[object & GameObjectIndexMask];
}
