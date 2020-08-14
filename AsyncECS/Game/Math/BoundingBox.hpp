//
//  BoundingBox.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>

namespace Game {

    class BoundingBox {
    public:
        glm::vec2 center;
        glm::vec2 extends;
        
        BoundingBox();
        BoundingBox(const BoundingBox& other);
        BoundingBox(const glm::vec2& center, const glm::vec2& extends);
        ~BoundingBox();
        
        bool Intersects(const BoundingBox& other) const;
        bool Contains(const BoundingBox& other) const;
        float Area() const;

        bool operator==(const BoundingBox &other);
        bool operator!=(const BoundingBox &other);

        BoundingBox CreateWorldAligned(const glm::mat3x3& matrix) const;
    };
}
