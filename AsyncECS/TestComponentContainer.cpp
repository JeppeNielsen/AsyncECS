//
//  TestComponentContainer.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 22/10/2019.
//  Copyright © 2019 Jeppe Nielsen. All rights reserved.
//

#include "ComponentContainer.hpp"

struct Touchable {
    int Down;
    int Up;
};

int main_containers() {

    AsyncECS::ComponentContainer<Touchable> container;
    
    container.Create(10);
    container.Reference(5,10);
    container.Destroy(10);
    container.Destroy(5);

    return 0;
}
