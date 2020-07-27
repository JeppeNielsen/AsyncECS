//
//  RenderSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "RenderSystem.hpp"

using namespace Game;
using namespace AsyncECS;

void RenderSystem::Initialize(QuadTreeSystem &quadTreeSystem) {
    this->quadTreeSystem = &quadTreeSystem;
}

void RenderSystem::Update(const WorldTransform &transform, const Camera &camera) {
    
    BoundingBox boundingBox;
    boundingBox.center = 0.0f;
    boundingBox.extends = camera.ViewSize;
    auto cameraBounds = boundingBox.CreateWorldAligned(transform.world);
    
    std::vector<GameObject> objectsInView;
    quadTreeSystem->Query(cameraBounds, objectsInView);
    
    std::cout << "Num objects in view : "<< objectsInView.size() << "\n";
    for (auto object : objectsInView){
        std::cout << "Object in view : "<< object << "\n";
    }
}
 
