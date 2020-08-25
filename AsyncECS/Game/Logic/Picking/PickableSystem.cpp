//
//  PickableSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 21/08/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "PickableSystem.hpp"

using namespace Game;
using namespace AsyncECS;

void PickableSystem::Initialize(PickableOctreeSystem &octreeSystem) {
    this->octreeSystem = &octreeSystem;
}

Ray GetRay(const WorldTransform &transform, const Camera &camera, const ivec2& screenSize, const ivec2& screenPosition) {
    //const Rect& viewPort = Viewport() * Screen::MainScreen->Size();
    
    const ivec2 center = screenSize / 2;
    
    vec2 fromCenter = screenPosition - center;
    fromCenter /= center;
    
    mat4x4 viewProjection = transform.worldInverse * camera.GetProjection();
    viewProjection = inverse(viewProjection);
    
    vec4 rayStartPosition = vec4(fromCenter.x,fromCenter.y,-1.0f,1.0f);
    vec4 rayEndPosition = vec4(fromCenter.x,fromCenter.y,1.0f,1.0f);
    
    rayStartPosition = viewProjection * rayStartPosition;
    rayEndPosition = viewProjection * rayEndPosition;
    
    return Ray(rayStartPosition, rayEndPosition - rayStartPosition);
}

void PickableSystem::Update(const WorldTransform &transform, const Camera &camera, const Input& input) {
    
    for(auto& resetPickable : resetPickables) {
        GetComponents(resetPickable.gameObject, [](Pickable& pickable) {
            pickable.down.clear();
            pickable.up.clear();
            pickable.clicked.clear();
        });
    }
    
    for(auto& touchDown : input.touchesDown) {
        TouchDown(transform, camera, input, touchDown);
    }
    
    resetPickables = activePickables;
    
    for(auto& touchUp : input.touchesUp) {
        TouchUp(transform, camera, input, touchUp);
    }
}
                 
void PickableSystem::TouchDown(const WorldTransform &transform, const Camera &camera, const Input &input, const InputTouch& touch) {
    
    auto touchPosition = input.touchPosition[touch.index];
    
    Ray ray = GetRay(transform, camera, screenSize, touchPosition.position);
    
    std::vector<GameObject> downs;
    octreeSystem->Query(ray, downs);
    
    
    
    for(auto down : downs) {
        
        PickEvent e;
        e.touch = touch;
        e.touchPosition = touchPosition;
        e.worldPosition = {0.0f, 0.0f, 0.0f };
        e.object = down;
        
        GetComponents(down, [&input, &touch, &e](Pickable& pickable) {
           pickable.down.push_back(e);
        });
        activePickables.push_back({ touch, down });
    }
}
                            
void PickableSystem::TouchUp(const WorldTransform &transform, const Camera &camera, const Input &input, const InputTouch& touch) {
    
    auto touchPosition = input.touchPosition[touch.index];
    
    Ray ray = GetRay(transform, camera, screenSize, touchPosition.position);
    
    std::vector<GameObject> ups;
    octreeSystem->Query(ray, ups);
    
    
    
    for(auto& activePickable : activePickables) {
        if (activePickable.touch.index == touch.index) {
            
            PickEvent e;
            e.touch = touch;
            e.touchPosition = input.touchPosition[touch.index];
            e.worldPosition = {0,0,0};
            e.object = activePickable.gameObject;
            
            bool clicked = std::find(ups.begin(), ups.end(), activePickable.gameObject) != ups.end();
            GetComponents(activePickable.gameObject, [&touch, &e, &ups, clicked] (Pickable& pickable) {
                pickable.up.push_back(e);
                if (clicked) {
                    pickable.clicked.push_back(e);
                }
            });
        }
    }
    
    activePickables.erase(std::remove_if(activePickables.begin(), activePickables.end(), [&touch](auto& activePickable) {
        return activePickable.touch.index == touch.index;
    }), activePickables.end());
    
}
                            
