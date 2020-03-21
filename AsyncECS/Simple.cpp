#include <iostream>
#include "AllTests.hpp"
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

struct Limit {
    float x;
    float y;
};

struct Renderable {
    int imageNo;
};

struct MovementSystem : System<Position, const Velocity> {
    void Update(Position& position, const Velocity& velocity) {
        position.x += velocity.x;
        position.y += velocity.y;
        std::cout << "Movement: "<< position.x << ", " << position.y <<std::endl;
    }
};

struct RenderSystem : System<const Position, const Renderable> {
    void Update(const Position& position, const Renderable& renderable) {
        DrawRenderable(position, renderable);
    }
    
    void DrawRenderable(const Position& position, const Renderable& renderable) {
        std::cout << "Render: "<< position.x << ", " << position.y <<std::endl;
    }
};

struct LimitSystem : SystemChangedGameObject<const Position, const Limit>, SceneModifier<Limit, Position> {
    void Update(GameObject gameObject, const Position& position, const Limit& limit) {
        if (position.x>=limit.x) {
            Modify([gameObject] (Modifier& modifier) {
                modifier.RemoveGameObject(gameObject);
                //auto createdGameObject = modifier.CreateGameObject();
                
                //std::cout << createdGameObject << std::endl;
                //modifier.AddComponent<Limit>(createdGameObject, 0.0f, 0.0f);
                //modifier.AddComponent<Position>(createdGameObject);
                
            });
            
        }
        std::cout << "LimitSystem : "<<gameObject << std::endl;
    }
};

int main_simple() {
    using Components = ComponentTypes<Position, Velocity, Renderable, Limit>;
    using Systems = SystemTypes<MovementSystem, RenderSystem, LimitSystem>;
    using Registry = Registry<Components>;

    Registry registry;
    Scene<Registry, Systems> scene(registry);
    
    auto gameObject = scene.CreateGameObject();
    scene.AddComponent<Position>(gameObject, 0.0f, 0.0f);
    scene.AddComponent<Velocity>(gameObject, 1.0f, 2.0f);
    scene.AddComponent<Renderable>(gameObject);
    scene.AddComponent<Limit>(gameObject, 3.0f, 0.0f);
    
    for (int i=0; i<10; i++) {
        scene.Update();
        if (registry.IsGameObjectValid(gameObject)) {
            auto& p = scene.GetComponent<Position>(gameObject);
            std::cout << "Position " << p.x << " " << p.y << std::endl;
        }
    }
    //std::cout << " graph: "<<std::endl;
    //scene.WriteGraph(std::cout);
    return 0;
}
