//
//  GameObjectDatabase.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 08/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include "GameObjectDatabase.hpp"

using namespace AsyncECS;

bool GameObjectDatabase::IsValid(const GameObject gameObject) const {
    const auto pos = gameObject & GameObjectIndexMask;
    return (pos < objects.size() && objects[pos] == gameObject);
}

GameObject GameObjectDatabase::Create() {
    GameObject object;

    if(available) {
        const auto objectIndex = next;
        const auto version = objects[objectIndex] & (~GameObjectIndexMask);

        object = objectIndex | version;
        next = objects[objectIndex] & GameObjectIndexMask;
        objects[objectIndex] = object;
        --available;
    } else {
        object = GameObject(objects.size());
        objects.push_back(object);
    }
    return object;
}

void GameObjectDatabase::Remove(const GameObject gameObject) {
    const auto object = gameObject & GameObjectIndexMask;
    const auto version = (((gameObject >> 20) + 1) & GameObjectIndexMask) << 20;
    const auto node = (available ? next : ((object + 1) & GameObjectIndexMask)) | version;

    objects[object] = node;
    next = object;
    ++available;
}

size_t GameObjectDatabase::Size() const {
    return objects.size();
}

