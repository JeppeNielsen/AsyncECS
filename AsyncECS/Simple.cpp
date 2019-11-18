#include <iostream>
#include "Scene.hpp"

#include <variant>

using namespace AsyncECS;

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct MovementSystem : System<Position, const Velocity>, NoDependencies, NoComponentView {
    void Initialize() { }
    void Update(Position& position, const Velocity& velocity) {
        position.x += velocity.x;
        position.y += velocity.y;
    }
};

int main() {
    using Components = ComponentTypes<Position, Velocity>;
    using Systems = SystemTypes<MovementSystem>;
    using RegistryType = Registry<Components>;
    
    RegistryType registry;
    Scene<RegistryType, Systems> scene(registry);
    
    auto gameObject = scene.CreateGameObject();
    scene.AddComponent<Position>(gameObject, 0.0f, 0.0f);
    scene.AddComponent<Velocity>(gameObject, 1.0f, 2.0f);
    
    scene.Update();
    
    auto& p = scene.GetComponent<Position>(gameObject);
    std::cout << "Position " << p.x << " " << p.y << std::endl;
    
    return 0;
}
