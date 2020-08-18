//
//  WorldBoundingBoxSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once

#include "ECS.hpp"
#include "WorldTransform.hpp"
#include "LocalBoundingBox.hpp"
#include "WorldBoundingBox.hpp"

namespace Game {
    struct WorldBoundingBoxSystem : AsyncECS::SystemChanged<const WorldTransform, const LocalBoundingBox, WorldBoundingBox> {
        void Changed(const WorldTransform& worldTransform,
                    const LocalBoundingBox& localBoundingBox,
                     WorldBoundingBox& worldBoundingBox);
        
        void Update(const WorldTransform& worldTransform,
                    const LocalBoundingBox& localBoundingBox,
                    WorldBoundingBox& worldBoundingBox);
        
        constexpr int EnableConcurrency() { return 5000; }
    };
}
