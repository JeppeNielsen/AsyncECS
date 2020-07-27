//
//  RenderSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "ECS.hpp"
#include "WorldTransform.hpp"
#include "Camera.hpp"
#include "QuadTreeSystem.hpp"

namespace Game {
struct RenderSystem :
AsyncECS::System<const WorldTransform, const Camera>,
AsyncECS::SystemDependencies<QuadTreeSystem>
{
    void Initialize(QuadTreeSystem& quadTreeSystem);
    void Update(const WorldTransform& transform, const Camera& camera);

    QuadTreeSystem* quadTreeSystem;
};
}
