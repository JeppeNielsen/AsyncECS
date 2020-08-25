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
#include "OctreeSystem.hpp"
#include "Mesh.hpp"
#include "VertexRenderer.hpp"
#include "Shader.hpp"
#include "TaskRunner.hpp"

namespace Game {

using RenderOctreeSystem = OctreeSystem<const WorldTransform, const Mesh>;

struct RenderSystem :
    AsyncECS::System<const WorldTransform, const Camera>,
    AsyncECS::SystemDependencies<RenderOctreeSystem>,
    AsyncECS::ComponentView<const WorldTransform, const Mesh> {
        void Initialize(RenderOctreeSystem& octreeSystem);
        void Update(const WorldTransform& transform, const Camera& camera);
        
        void RenderScene();
        
        VertexRenderer<Vertex> vertexRenderer;
        Shader<Vertex> shader;

        RenderOctreeSystem* octreeSystem;
        
        WorldTransform cameraTransform;
        Camera camera;
        
        using Meshes = std::vector<Mesh>;
        
        Meshes worldSpaceMeshes;
        
        void CalculateWorldSpaceMesh(const std::vector<AsyncECS::GameObject>& objects, const int startIndex, const int count, Mesh& worldSpaceMesh);
        
        void CalculateWorldSpaceMesh(const AsyncECS::GameObject gameObject, Mesh& worldSpaceMesh);
        
        AsyncECS::TaskRunner taskRunner;
        
    };
}
