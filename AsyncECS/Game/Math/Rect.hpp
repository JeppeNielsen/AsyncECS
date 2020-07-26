//
//  Rect.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Vector2.hpp"
#include "Matrix3x3.hpp"
#include <math.h>

namespace Game {
    struct Rect {
        Vector2 min;
        Vector2 max;
        
        inline bool Overlaps(const Rect& other) const {
            return max.x > other.min.x ||
                other.max.x < min.x ||
                max.y > other.max.y ||
            min.y < other.max.y;
        }
        
        inline bool operator ==(const Rect &other) const{
            return !(min!=other.min || max!=other.max);
        }
        
        inline  bool operator !=(const Rect &other) const{
            return (min!=other.min || max!=other.max);
        }
        
        inline Vector2 Position() const {
            return min;
        }
        
        inline Vector2 Size() const {
            return max - min;
        }
        
        inline Vector2 Center() const {
            return (min + max) * 0.5f;
        }
        
        Rect CreateWorldAligned(const Matrix3x3& matrix) const {
            
            Rect rect;
            
            Vector2 extends = max - min;
            
            Vector2 halfExtends = extends * 0.5f;

            rect.min = matrix.TransformPoint(min);
            
            extends.x = (fabsf(matrix.m[0][0]) * halfExtends.x + fabsf(matrix.m[0][1]) * halfExtends.y);
            extends.y = (fabsf(matrix.m[1][0]) * halfExtends.x + fabsf(matrix.m[1][1]) * halfExtends.y);
            
            rect.max = min + extends * 2.0f;
            
            return rect;
        }
    };
}
