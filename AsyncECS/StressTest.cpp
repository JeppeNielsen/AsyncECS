//
//  StressTest.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 28/05/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "ECS.hpp"
#include "Math.hpp"
#include "Timer.hpp"
#include <iostream>



using namespace AsyncECS;
using namespace Game;

struct Position {
    vec2 position;
};

struct Velocity {
    vec2 velocity;
};

struct Rotation {
    float angle;
};

struct AngularRotation {
    float speed;
};

struct Bouncer {
    float position;
};

struct VelocitySystem : System<Position, const Velocity> {
    void Update(Position& position, const Velocity& velocity) {
        position.position += velocity.velocity;
    }
};

struct RotationSystem : System<Rotation, const AngularRotation> {
    void Update(Rotation& rotation, const AngularRotation& angularRotation) {
        
        for (int i=0; i<1000; i++) {
            rotation.angle += angularRotation.speed * i;
        }
    }
};

struct BounceSystem : System<Bouncer, const Velocity> {
    void Update(Bouncer& bouncer, const Velocity& velocity) {
        
        for (int i=0; i<1000; i++) {
            bouncer.position += length(velocity.velocity);
        }
    }
};

int main_stress() {
    using Components = ComponentTypes<Position, Velocity, Rotation, AngularRotation, Bouncer>;
    using Registry = Registry<Components>;
    
    using Systems = SystemTypes<VelocitySystem, RotationSystem, BounceSystem>;
    using Scene = Scene<Registry, Systems>;
    
    Registry registry;
    Scene scene(registry);
    
    {
    Timer timer;
    for (int i=0; i<100000; ++i) {
        auto gameObject1 = scene.CreateGameObject();
        scene.AddComponent<Position>(gameObject1);
        scene.AddComponent<Velocity>(gameObject1, vec2(i,3));
        
        scene.AddComponent<Rotation>(gameObject1);
        scene.AddComponent<AngularRotation>(gameObject1, (float)i);
        
        scene.AddComponent<Bouncer>(gameObject1, 10.0f);
    }
    auto time = timer.Stop();
    std::cout << "Create 1 mil : " << time<<"\n";
    }
        
    for (int i=0; i<10; i++) {
        Timer timer;
        scene.Update();
        auto time = timer.Stop();
    }
    
    scene.WriteGraph(std::cout);
    
    return 0;
}



