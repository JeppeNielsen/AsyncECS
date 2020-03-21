//
//  IScene.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 03/03/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "GameObject.hpp"

namespace AsyncECS {
    struct IScene {
        virtual GameObject CreateGameObject() = 0;
        virtual void RemoveGameObject(const GameObject gameObject) = 0;
        virtual void AddGameObjectToComponentContainer(const GameObject gameObject, const int index) = 0;
        virtual void RemoveGameObjectFromComponentContainer(const GameObject gameObject, const int index) = 0;
    };
}
