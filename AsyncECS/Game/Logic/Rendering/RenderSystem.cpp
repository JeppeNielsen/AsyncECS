//
//  RenderSystem.cpp
//  AsyncECS
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "RenderSystem.hpp"
#include "OpenGL.hpp"

using namespace Game;
using namespace AsyncECS;

void RenderSystem::Initialize(QuadTreeSystem &quadTreeSystem) {
    this->quadTreeSystem = &quadTreeSystem;
    

    shader.Load(
        SHADER_SOURCE(
            attribute vec4 Position;
            attribute vec4 Color;
            uniform mat4 ViewProjection;
            varying vec4 vColor;
            void main() {
               vColor = Color;
                gl_Position = Position * ViewProjection;
            }
        )
        ,
        SHADER_SOURCE(
            varying vec4 vColor;
            void main() {
                gl_FragColor = vColor;
            }
        )
    );
    
}

void RenderSystem::Update(const WorldTransform &transform, const Camera &camera) {
    
    BoundingBox boundingBox;
    boundingBox.center = 0.0f;
    boundingBox.extends = camera.ViewSize * 2;
    auto cameraBounds = boundingBox.CreateWorldAligned(transform.world);
    
    std::vector<GameObject> objectsInView;
    quadTreeSystem->Query(cameraBounds, objectsInView);
    
    //std::cout << "Num objects in view : "<< objectsInView.size() << "\n";
    vertexRenderer.BeginLoop();
    
    vertexIndex = 0;
    triangleIndex = 0;
    vertexRenderer.BeginLoop();
    for (auto object : objectsInView){
    //    std::cout << "Object in view : "<< object << "\n";
        Render(object);
    }
    
    cameraTransform = transform;
    this->camera = camera;
}
 
void RenderSystem::Render(const GameObject gameObject) {
    GetComponents(gameObject, [this] (const WorldTransform& worldTransform, const Mesh& mesh) {
        
        auto size = vertexRenderer.vertexIndex;
        for(int i=0; i<mesh.vertices.size(); ++i) {
            auto& source = mesh.vertices[i];
            auto& dest = vertexRenderer.vertices[vertexRenderer.vertexIndex];
            
            dest.Position = worldTransform.world.TransformPoint(source.Position);
            dest.TextureCoords = source.TextureCoords;
            dest.Color = source.Color;
            
            vertexRenderer.vertexIndex++;
        }
        
        for(int i=0; i<mesh.triangles.size(); ++i) {
            vertexRenderer.triangles[vertexRenderer.triangleIndex] = mesh.triangles[i] + size;
            vertexRenderer.triangleIndex++;
        }
    });
}

void RenderSystem::RenderScene() {
    
    Matrix4x4 projection;
    projection.InitOrthographic(0, camera.ViewSize.y * 2, camera.ViewSize.x * 2, 0, -0.1f, 10.0f);
    
    Matrix3x3 cameraInverse = cameraTransform.worldInverse;
    Matrix4x4 inverse = Matrix4x4::IDENTITY;
    for (int x=0; x<2; x++) {
        for(int y=0; y<2; y++ ) {
            inverse.m[x][y] = cameraInverse.m[x][y];
        }
    }
    inverse.SetTranslation(cameraInverse.Position());
    
    Matrix4x4 viewProjection = projection.Multiply(inverse);
    
    
    shader.Use();
    shader.SetViewProjection(viewProjection.GetGlMatrix());
    std::cout << vertexRenderer.vertexIndex << "\n";
    
    vertexRenderer.Render();
    
    
    
}
