//
//  AssignChildrenSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 23/02/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "HierarchySystem.hpp"

using namespace Game;
using namespace AsyncECS;

void HierarchySystem::Initialize() {

}

void HierarchySystem::Update(GameObject gameObject, Hierarchy& hierarchy) {
    auto previousParent = hierarchy.previousParent;
    auto currentParent = hierarchy.parent;
    
    if (previousParent != GameObjectNull) {
       GetComponents(previousParent, [gameObject](Hierarchy& hierarchy) {
           auto& children = hierarchy.children;
           children.erase(std::find(children.begin(), children.end(), gameObject));
       });
    }
    
    if (currentParent != GameObjectNull) {
       GetComponents(currentParent, [gameObject](Hierarchy& hierarchy) {
           hierarchy.children.push_back(gameObject);
       });
    }
    
    hierarchy.previousParent = currentParent;
}
