//
//  BoundingBox.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "BoundingBox.hpp"
#include "Matrix3x3.hpp"

using namespace Game;

BoundingBox::BoundingBox() {
}

BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox(const BoundingBox& other) {
    center = other.center;
    extends = other.extends;
}

BoundingBox::BoundingBox(const Vector2& center, const Vector2& extends) {
    this->center = center;
    this->extends = extends;
}

bool BoundingBox::Intersects(const BoundingBox& other) const {
    const Vector2 v = other.center - center;
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
    
    Vector2 min1 = center - extends * 0.5f;
    Vector2 max1 = center + extends * 0.5f;

    Vector2 min2 = other.center - other.extends * 0.5f;
    Vector2 max2 = other.center + other.extends * 0.5f;

    if (min2.x<min1.x) return false;
    if (max2.x>max1.x) return false;

    if (min2.y<min1.y) return false;
    if (max2.y>max1.y) return false;

    return true;
};

BoundingBox BoundingBox::CreateWorldAligned(const Matrix3x3& matrix) const {
    BoundingBox boundingBox;
    
    Vector2 halfExtends = extends * 0.5f;

    boundingBox.center = matrix.TransformPoint(center);
    
    Vector2 extends;

    extends.x = (fabsf(matrix.m[0][0]) * halfExtends.x + fabsf(matrix.m[0][1]) * halfExtends.y);
    extends.y = (fabsf(matrix.m[1][0]) * halfExtends.x + fabsf(matrix.m[1][1]) * halfExtends.y);
    
    boundingBox.extends = extends * 2.0f;

    return boundingBox;
}
