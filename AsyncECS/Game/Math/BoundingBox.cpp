//
//  BoundingBox.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "BoundingBox.hpp"
#include <glm/mat3x3.hpp>

using namespace Game;
using namespace glm;

BoundingBox::BoundingBox() {
}

BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox(const BoundingBox& other) {
    center = other.center;
    extends = other.extends;
}

BoundingBox::BoundingBox(const vec2& center, const vec2& extends) {
    this->center = center;
    this->extends = extends;
}

bool BoundingBox::Intersects(const BoundingBox& other) const {
    const vec2 v = other.center - center;
    if (fabs(v.x)>(extends.x + other.extends.x)) return false;
    if (fabs(v.y)>(extends.y + other.extends.y)) return false;
    return true;
}

float BoundingBox::Area() const {
    return extends.x * extends.y;
}

bool BoundingBox::operator ==(const BoundingBox &other) {
    if (extends!=other.extends) return false;
    if (center!=other.center) return false;
    return true;
}

bool BoundingBox::operator !=(const BoundingBox &other) {
    if (extends!=other.extends) return true;
    if (center!=other.center) return true;
    return false;
}

bool BoundingBox::Contains(const BoundingBox& other) const {
    
    vec2 min1 = center - extends * 0.5f;
    vec2 max1 = center + extends * 0.5f;

    vec2 min2 = other.center - other.extends * 0.5f;
    vec2 max2 = other.center + other.extends * 0.5f;

    if (min2.x<min1.x) return false;
    if (max2.x>max1.x) return false;

    if (min2.y<min1.y) return false;
    if (max2.y>max1.y) return false;

    return true;
};

BoundingBox BoundingBox::CreateWorldAligned(const mat3x3& matrix) const {
    BoundingBox boundingBox;
    
    vec2 halfExtends = extends * 0.5f;
    
    vec3 pos3d = {center.x, center.y , 0.0f};

    boundingBox.center = matrix * pos3d; //matrix.TransformPoint(center);
    
    vec2 extends;

    extends.x = (fabsf(matrix[0][0]) * halfExtends.x + fabsf(matrix[0][1]) * halfExtends.y);
    extends.y = (fabsf(matrix[1][0]) * halfExtends.x + fabsf(matrix[1][1]) * halfExtends.y);
    
    boundingBox.extends = extends * 2.0f;

    return boundingBox;
}
