//
//  BoundingBox.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Vector2.hpp"

namespace Game {
    class Matrix3x3;
    class BoundingBox {
    public:
        Vector2 center;
        Vector2 extends;
        
        BoundingBox();
        BoundingBox(const BoundingBox& other);
        BoundingBox(const Vector2& center, const Vector2& extends);
        ~BoundingBox();
        
        bool Intersects(const BoundingBox& other) const;
        bool Contains(const BoundingBox& other) const;
        float Area() const;

        bool operator==(const BoundingBox &other);
        bool operator!=(const BoundingBox &other);

        BoundingBox CreateWorldAligned(const Matrix3x3& matrix) const;
    };
}
