//
//  RenderSystem.hpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "ECS.hpp"
#include "WorldTransform.hpp"
#include "Camera.hpp"
#include "QuadTreeSystem.hpp"
#include "Mesh.hpp"
#include "VertexRenderer.hpp"
#include "Shader.hpp"

namespace Game {
struct RenderSystem :
AsyncECS::System<const WorldTransform, const Camera>,
AsyncECS::SystemDependencies<QuadTreeSystem>,
AsyncECS::ComponentView<const WorldTransform, const Mesh>
{
    void Initialize(QuadTreeSystem& quadTreeSystem);
    void Update(const WorldTransform& transform, const Camera& camera);
    void Render(const AsyncECS::GameObject gameObject);
    
    void RenderScene();
    
    VertexRenderer<Vertex> vertexRenderer;
    Shader<Vertex> shader;
    int vertexIndex;
    int triangleIndex;
    QuadTreeSystem* quadTreeSystem;
    
    WorldTransform cameraTransform;
    Camera camera;
};
}
