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
    boundingBox.center = {0.0f,0.0f};
    boundingBox.extends = camera.ViewSize * 2.0f;
    auto cameraBounds = boundingBox.CreateWorldAligned(transform.world);
    
    std::vector<GameObject> objectsInView;
    quadTreeSystem->Query(cameraBounds, objectsInView);
    
    std::cout << "num quads : " << objectsInView.size() << "\n";
    
    //std::cout << "Num objects in view : "<< objectsInView.size() << "\n";
    
    worldSpaceMeshes.clear();
    
    //7500; == 6-7 ms
    
    const int chunkSize = 7000;
    
    int numMeshes = (int)ceil(objectsInView.size() / (double)chunkSize);
    
    worldSpaceMeshes.resize(numMeshes);
    
    int meshIndex = 0;
    for (int i=0; i<objectsInView.size(); i+=chunkSize) {
        
        auto& worldSpaceMesh = worldSpaceMeshes[meshIndex++];
        
        int fromIndex = i;
        int toIndex = std::min((int)objectsInView.size(),  fromIndex + chunkSize);
        taskRunner.RunTask([this, &objectsInView, &worldSpaceMesh, fromIndex, toIndex] () {
            CalculateWorldSpaceMesh(objectsInView, fromIndex, toIndex, worldSpaceMesh);
        });
    }
    
    while(taskRunner.Update());
    
    cameraTransform = transform;
    this->camera = camera;
}

void RenderSystem::RenderScene() {
    
    Matrix4x4 projection;
    projection.InitOrthographic(0, camera.ViewSize.y * 2, camera.ViewSize.x * 2, 0, -0.1f, 10.0f);
    
    glm::mat3x3 cameraInverse = cameraTransform.worldInverse;
    Matrix4x4 inverse = Matrix4x4::IDENTITY;
    for (int x=0; x<2; x++) {
        for(int y=0; y<2; y++ ) {
            inverse.m[x][y] = cameraInverse[x][y];
        }
    }
    inverse.SetTranslation({cameraInverse[2][0], cameraInverse[2][1],0});
    
    Matrix4x4 viewProjection = projection.Multiply(inverse);
    
    
    shader.Use();
    shader.SetViewProjection(viewProjection.GetGlMatrix());
    
    
    
   // vertexRenderer.RenderVertices(vertices, 0, vertices.size(), triangles, 0, triangles.size());
    
    for(auto& worldSpaceMesh : worldSpaceMeshes) {
        vertexRenderer.RenderVertices(worldSpaceMesh.vertices, 0, worldSpaceMesh.vertices.size(), worldSpaceMesh.triangles, 0, worldSpaceMesh.triangles.size());
    }
    
}

void RenderSystem::CalculateWorldSpaceMesh(const std::vector<GameObject> &objects, const int startIndex, const int endIndex, Mesh &worldSpaceMesh) {
    for (int i=startIndex; i<endIndex; ++i) {
        CalculateWorldSpaceMesh(objects[i], worldSpaceMesh);
    }
}

void RenderSystem::CalculateWorldSpaceMesh(const AsyncECS::GameObject gameObject, Mesh &worldSpaceMesh) {
    GetComponents(gameObject, [gameObject, &worldSpaceMesh] (const WorldTransform& worldTransform, const Mesh& mesh) {
        
        GLshort baseTriangleIndex = (GLshort)worldSpaceMesh.vertices.size();
        for(int i=0; i<mesh.vertices.size(); ++i) {
            auto& source = mesh.vertices[i];
            Vertex dest;
            
            const glm::vec3 pos3d = {source.Position.x, source.Position.y,1.0f};
            
            dest.Position = worldTransform.world * pos3d;//worldTransform.world.TransformPoint(source.Position);
            dest.TextureCoords = source.TextureCoords;
            dest.Color = source.Color;
            
            worldSpaceMesh.vertices.emplace_back(dest);
        }
        
        for(int i=0; i<mesh.triangles.size(); ++i) {
            worldSpaceMesh.triangles.emplace_back(baseTriangleIndex + mesh.triangles[i]);
        }
    });
}
