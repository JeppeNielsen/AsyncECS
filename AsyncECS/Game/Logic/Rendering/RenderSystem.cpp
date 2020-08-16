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
                gl_Position = ViewProjection * Position;
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
    
    
    mat4x4 projection = ortho(0.0f, camera.ViewSize.x * 2.0f, 0.0f, camera.ViewSize.y * 2.0f, -0.1f, 10.0f);
    
    glm::mat3x3 cameraInverse = cameraTransform.worldInverse;
    mat4x4 inverse = mat4x4(1.0f);
    for (int x=0; x<2; x++) {
        for(int y=0; y<2; y++ ) {
            inverse[x][y] = cameraInverse[x][y];
        }
    }
    inverse[3][0] = cameraInverse[2][0];
    inverse[3][1] = cameraInverse[2][1];
    
    mat4x4 viewProjection = projection * inverse;
    
    shader.Use();
    shader.SetViewProjection(glm::value_ptr(viewProjection));
    
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
            
            const glm::vec3 pos3d = vec3(source.Position, 1.0f);
            dest.Position = worldTransform.world * pos3d;
            
            dest.TextureCoords = source.TextureCoords;
            dest.Color = source.Color;
            
            worldSpaceMesh.vertices.emplace_back(dest);
        }
        
        for(int i=0; i<mesh.triangles.size(); ++i) {
            worldSpaceMesh.triangles.emplace_back(baseTriangleIndex + mesh.triangles[i]);
        }
    });
}
