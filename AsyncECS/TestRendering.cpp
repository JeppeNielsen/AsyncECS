//
//  TestRendering.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 30/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "Engine.hpp"
#include "OpenGL.hpp"
#include "ECS.hpp"
#include "RenderSystem.hpp"
#include "WorldBoundingBoxSystem.hpp"
#include "WorldTransformSystem.hpp"
#include "HierarchySystem.hpp"
#include "InputSystem.hpp"
#include "PickableSystem.hpp"
#include "LogicTests.hpp"

using namespace Game;

struct ClickScaler {
    float amountToScale;
    
};

struct ClickScalerSystem : AsyncECS::SystemChanged<const ClickScaler, const Pickable, LocalTransform> {
    
    void Changed(const ClickScaler& scaler, const Pickable& pickable, LocalTransform& localTransform) {
    
    }
    
    void Update(const ClickScaler& scaler, const Pickable& pickable, LocalTransform& localTransform ) {
        
        for(auto& click : pickable.clicked) {
            if (click.touch.index == 0) {
                localTransform.scale += scaler.amountToScale;
            } else {
                localTransform.scale -= scaler.amountToScale;
            }
        }
    }
    
};


struct Rotator {
    float speed;
};

struct RotatorSystem : AsyncECS::System<LocalTransform, const Rotator> {
    void Update(LocalTransform& local, const Rotator& rotator) {
        local.rotation *= glm::angleAxis(glm::radians(rotator.speed), vec3(0,0,1));
    }
    
    constexpr int EnableConcurrency() { return 5000; }
};

struct Colorizer {
    float hue;
    float speed;
};

struct ColorizerSystem : AsyncECS::System<Mesh, Colorizer> {
    void Update(Mesh& mesh, Colorizer& colorizer) {
        colorizer.hue += colorizer.speed;
        double dHue = 360.0 / mesh.vertices.size();
        for (int i=0; i<mesh.vertices.size(); ++i) {
            mesh.vertices[i].Color = Color::HslToRgb(colorizer.hue + dHue * i, 1.0, 1.0, 1.0);
        }
    }
};


struct State : IState {
    using ComponentTypes = AsyncECS::ComponentTypes<
        WorldBoundingBox, LocalBoundingBox, WorldTransform, Camera, Mesh, LocalTransform, Hierarchy, Rotator,
        Colorizer, Input, Pickable, ClickScaler
    >;
    using Systems = AsyncECS::SystemTypes<
        RenderOctreeSystem, RenderSystem, WorldBoundingBoxSystem, HierarchySystem, WorldTransformSystem, RotatorSystem,
        ColorizerSystem, InputSystem, PickableSystem, ClickScalerSystem
    >;
    
    using Registry = AsyncECS::Registry<ComponentTypes>;
    using Scene = AsyncECS::Scene<Registry, Systems>;

    Registry registry;
    std::shared_ptr<Scene> scene;
    AsyncECS::GameObject meshObject;
    ivec2 camPos;
    
    void Initialize() override {
        
        Game::Tests::LogicTests logicTests;
        logicTests.Run();
        
        scene = std::make_shared<Scene>(registry);
        scene->GetSystem<InputSystem>().SetDevice(device.Input.Device());
        scene->GetSystem<PickableSystem>().screenSize = device.Screen.Size;
        
        auto cameraGO = scene->CreateGameObject();
        scene->AddComponent<WorldTransform>(cameraGO, glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f,0.0f,0.0f)));
        scene->AddComponent<Camera>(cameraGO, glm::vec2(8,8), -0.1f, 10.0f);
        scene->AddComponent<LocalTransform>(cameraGO);
        scene->AddComponent<Hierarchy>(cameraGO);
        scene->AddComponent<Input>(cameraGO);
        
        //scene->AddComponent<Rotator>(cameraGO, -0.5f);
        
        meshObject = CreateMesh();
        
        auto quad1 = CreateQuad({0,0,0}, {1.0f,1.0f,1.0f}, false);
        
        
        //auto quad2 = CreateQuad({2.0f,0,0}, {1.0f,1.0f,0.0f}, false, quad1);
            
        
        for (int x=0; x<200; x++) {
            for (int y=0; y<200; y++) {
                auto q = CreateQuad({x*1.0f,y*1.0f,0.0f}, {0.7f,0.7f,1.0f}, false, true ? quad1 : AsyncECS::GameObjectNull);
            
                if (x == 4 && y == 4) {
                    scene->AddComponent<Rotator>(q, 1.0f);
                }
            }
        }
        
        scene->WriteGraph(std::cout);
        
        camPos = {0,0};
        device.Input.ButtonDown.Bind([this](ButtonEvent button) {
            if (button.Id == "A") {
                camPos.x+=1;
            }
            if (button.Id == "Q") {
                camPos.y+=1;
            }
        });
    }
    
    AsyncECS::GameObject CreateMesh() {
        auto quadGO = scene->CreateGameObject();
        scene->AddComponent<Mesh>(quadGO);
        auto& mesh = scene->GetComponent<Mesh>(quadGO);
        
        mesh.vertices.push_back({ { -0.5f,-0.5f,0.0f }, {0,0}, Color(1.0f, 0.0f, 0.0f) });
        mesh.vertices.push_back({ { 0.5f,-0.5f,0.0f }, {1,0}, Color(1.0f, 0.0f, 0.0f) });
        mesh.vertices.push_back({ { 0.5f,0.5f,0.0f }, {1,1}, Color(1.0f, 0.0f, 0.0f) });
        mesh.vertices.push_back({ { -0.5f,0.5f,0.0f }, {0,1}, Color(1.0f, 0.0f, 0.0f) });
        
        mesh.triangles.push_back(0);
        mesh.triangles.push_back(1);
        mesh.triangles.push_back(2);
        
        mesh.triangles.push_back(0);
        mesh.triangles.push_back(2);
        mesh.triangles.push_back(3);

        //scene->AddComponent<Colorizer>(quadGO, 1.0f, 2.0f);
        
        return quadGO;
    }
    
    AsyncECS::GameObject CreateQuad(const glm::vec3& pos, const glm::vec3& scale, bool rotate, AsyncECS::GameObject parent = AsyncECS::GameObjectNull) {
        
        auto quadGO = scene->CreateGameObject();
        
        LocalTransform local;
        local.position = pos;
        local.scale = scale;
        
        scene->AddComponent<LocalTransform>(quadGO, local);
        scene->AddComponent<WorldTransform>(quadGO);
        scene->AddComponent<Hierarchy>(quadGO);
        scene->ReferenceComponent<Mesh>(quadGO, meshObject);
        
        scene->GetComponent<Hierarchy>(quadGO).parent = parent;
        
        BoundingBox boundingBox;
        boundingBox.center = {0,0,0};
        boundingBox.extends = {0.5f,0.5f,0.5f};
        
        scene->AddComponent<LocalBoundingBox>(quadGO, boundingBox);
        scene->AddComponent<WorldBoundingBox>(quadGO);
        
        if (rotate) {
            scene->AddComponent<Rotator>(quadGO, 1.0f);
        }
        
        scene->AddComponent<Pickable>(quadGO);
        scene->AddComponent<ClickScaler>(quadGO, 0.1f);

        return quadGO;
    }
    
    
    void Update(float dt) override {
        scene->Update();
        auto mousePos = device.Input.GetTouchPosition(0);
        
        glm::vec2 screenSize = {device.Screen.Size().x, device.Screen.Size().y};
        glm::vec2 mPos = {mousePos.x, mousePos.y};
        
        //vec2 pos =((screenSize * 0.5f - mPos) - screenSize*0.5f) * 0.01f;
        
        vec2 pos = mPos * 0.005f;
        scene->GetComponent<LocalTransform>(0).position = vec3(pos,0);
        
        //std::cout <<"fps: " << 1.0f / dt<< "\n";
        
        
    }
    
    void Render() override {
        glClearColor(100/255.0f, 149/255.0f, 237/255.0f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Game::Timer timer;
        timer.Start();
        scene->GetSystem<RenderSystem>().RenderScene();
        //std::cout << "Rendering :  " << timer.Stop() * 1000.0f<<"\n";
        
    }
};



int main() {
    Engine e;
    e.Start<State>();
    return 0;
}
