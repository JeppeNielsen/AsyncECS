# AsyncECS
Compile time asynchronous ECS in modern c++ 17

```c++
#include <iostream>
#include "Scene.hpp"

using namespace AsyncECS;

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct MovementSystem : System<Position, const Velocity> {
    void Initialize() { }
    void Update(Position& position, const Velocity& velocity) {
        position.x += velocity.x;
        position.y += velocity.y;
    }
};

int main() {
    using Components = ComponentTypes<Position, Velocity>;
    using Registry = Registry<Components>;
    using Systems = SystemTypes<MovementSystem>;
    using Scene = Scene<Registry, Systems>;
    
    Registry registry;
    Scene scene(registry);
    
    auto gameObject = scene.CreateGameObject();
    scene.AddComponent<Position>(gameObject, 0.0f, 0.0f);
    scene.AddComponent<Velocity>(gameObject, 1.0f, 2.0f);

    scene.Update();
    
    auto& p = scene.GetComponent<Position>(gameObject);
    std::cout << "Position " << p.x << " " << p.y << std::endl;
    
    return 0;
}
```
