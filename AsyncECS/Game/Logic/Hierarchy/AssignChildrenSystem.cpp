//
//  AssignChildrenSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 23/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "AssignChildrenSystem.hpp"


using namespace Game;

void AssignChildrenSystem::Initialize() {

}

void AssignChildrenSystem::Update(GameObject gameObject, const Parent &parent, Children &children) {
    auto previousParent = children.assignedParent;
    auto currentParent = parent.parent;
    
    if (previousParent!=GameObjectNull) {
       GetComponents(parent.parent, [previousParent](Children& children) {
           children.children.erase(std::find(children.children.begin(), children.children.end(), previousParent));
       });
    }
    
    if (currentParent!=GameObjectNull) {
        GetComponents(currentParent, [gameObject](Children& children) {
            children.children.push_back(gameObject);
        });
    }
    children.assignedParent = currentParent;
}
