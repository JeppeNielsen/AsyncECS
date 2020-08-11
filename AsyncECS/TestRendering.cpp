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

using namespace Game;

struct Rotator {
    float speed;
};

struct RotatorSystem : AsyncECS::System<LocalTransform, const Rotator> {
    void Update(LocalTransform& local, const Rotator& rotator) {
        local.rotation += rotator.speed * 0.016f;
    }
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
        Colorizer
    >;
    using Systems = AsyncECS::SystemTypes<
        QuadTreeSystem, RenderSystem, WorldBoundingBoxSystem, HierarchySystem, WorldTransformSystem, RotatorSystem,
        ColorizerSystem
    >;
    
    using Registry = AsyncECS::Registry<ComponentTypes>;
    using Scene = AsyncECS::Scene<Registry, Systems>;

    Registry registry;
    std::shared_ptr<Scene> scene;
    AsyncECS::GameObject meshObject;
    
    void Initialize() override {
        
        scene = std::make_shared<Scene>(registry);
        
        auto cameraGO = scene->CreateGameObject();
        scene->AddComponent<WorldTransform>(cameraGO, Matrix3x3::CreateTranslation({0,0}));
        scene->AddComponent<Camera>(cameraGO, Vector2(4,4));
        scene->AddComponent<LocalTransform>(cameraGO);
        scene->AddComponent<Hierarchy>(cameraGO);
        //scene->AddComponent<Rotator>(cameraGO, -0.5f);
        
        meshObject = CreateMesh();
        
        auto quad1 = CreateQuad({0,0}, 1.0f);
        CreateQuad({3,0}, 2.0f, quad1);
        
    }
    
    AsyncECS::GameObject CreateMesh() {
        auto quadGO = scene->CreateGameObject();
        scene->AddComponent<Mesh>(quadGO);
        auto& mesh = scene->GetComponent<Mesh>(quadGO);
        
        mesh.vertices.push_back({ { -0.5f,-0.5f }, {0,0}, Color(1.0f, 0.0f, 0.0f) });
        mesh.vertices.push_back({ { 0.5f,-0.5f }, {1,0}, Color(1.0f, 0.0f, 0.0f) });
        mesh.vertices.push_back({ { 0.5f,0.5f }, {1,1}, Color(1.0f, 0.0f, 0.0f) });
        mesh.vertices.push_back({ { -0.5f,0.5f }, {0,1}, Color(1.0f, 0.0f, 0.0f) });
        
        mesh.triangles.push_back(0);
        mesh.triangles.push_back(1);
        mesh.triangles.push_back(2);
        
        mesh.triangles.push_back(0);
        mesh.triangles.push_back(2);
        mesh.triangles.push_back(3);
        
        scene->AddComponent<Colorizer>(quadGO, 1.0f, 1.0f);
        
        return quadGO;
    }
    
    AsyncECS::GameObject CreateQuad(const Vector2& pos, const Vector2& scale, AsyncECS::GameObject parent = AsyncECS::GameObjectNull) {
        
        auto quadGO = scene->CreateGameObject();
        
        LocalTransform local;
        local.position = pos;
        local.rotation = 0.0f;
        local.scale = scale;
        
        scene->AddComponent<LocalTransform>(quadGO, local);
        scene->AddComponent<WorldTransform>(quadGO);
        scene->AddComponent<Hierarchy>(quadGO);
        scene->ReferenceComponent<Mesh>(quadGO, meshObject);
        
        scene->GetComponent<Hierarchy>(quadGO).parent = parent;
        
        BoundingBox boundingBox;
        boundingBox.center = {0,0};
        boundingBox.extends = {1,1};
        
        scene->AddComponent<LocalBoundingBox>(quadGO, boundingBox);
        scene->AddComponent<WorldBoundingBox>(quadGO);
        
        scene->AddComponent<Rotator>(quadGO, 1.0f);
        
        return quadGO;
    }
    
    
    void Update(float dt) override {
        scene->Update();
        auto mousePos = device.Input.GetTouchPosition(0);
        
        scene->GetComponent<LocalTransform>(0).position = (( device.Screen.Size() * 0.5f - mousePos) - device.Screen.Size()*0.5f) * 0.01f;
        
    }
    
    void Render() override {
        glClearColor(100/255.0f, 149/255.0f, 237/255.0f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene->GetSystem<RenderSystem>().RenderScene();
    }
};



int main() {
    Engine e;
    e.Start<State>();
    return 0;
}
