#pragma once

#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"

struct Platform
{
    glm::vec3 position;
    glm::vec3 scale;
};

struct Package{
    glm::vec3 position;
    bool isActive;
    int platformIndex;
};

struct DeliveryPoint{
    int platformIndex;
    bool isActive;
};

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        // Create Meshes functions
        void CreateDroneMeshes();
        void CreateGroundMesh(int rows, int columns);
        void CreateTreeMeshes();
        void GenerateRandomTrees(int numTrees);
        void CreatePlatformMesh();
        void CreateMinimapMarkerMesh();
        void CreateMinimapTreeMarkerMesh();
        void CreatePackageMesh();

        // Terrain functions
        float GetTerrainHeight(float x, float z);

        // Checkers
        bool CheckTreeOverlap(const glm::vec3 &position, float radius);
        bool CheckPlatformCollisions(const glm::vec3 &position, float platformSize);

        // Helper functions for minimap
        void RenderMeshWithCustomCamera(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix,
                                        implemented::Camera *customCamera, const glm::mat4 &customProjection);
        void RenderMainScene(float deltaTimeSeconds);

        // Package functions
        void SpawnPackageOnRandomPlatform();
        void RenderPackage(float deltaTimeSeconds);
        void RenderDirectionArrow();
        void UpdatePackageLogic();
        void SpawnDeliveryPoint();

    protected:
        implemented::Camera *camera;
        glm::mat4 projectionMatrix;
        float fov;
        float zNear;
        float zFar;

        // Drone parameters
        glm::vec3 dronePosition;
        glm::vec3 droneRotation;
        glm::vec3 droneForward;
        float propellerRotation;

        // Tree parameters
        std::vector<glm::vec3> treePositions;
        std::vector<float> treeScales;
        const float TREE_MIN_DISTANCE = 1.0f;
        const float MAP_SIZE = 50.0f;

        // Minimap parameters
        implemented::Camera *minimapCamera;
        glm::mat4 minimapProjectionMatrix;
        float minimapSize;
        const float MINIMAP_CAMERA_HEIGHT = 30.0f;

        // Matrices
        glm::mat4 modelMatrix;

        // Polygon mode
        GLenum polygonMode;

        // Meshes
        std::vector<Platform> platforms;

        // Package 
        Package currentPackage;
        float arrowRotation;
        const float ARROW_HEIGHT = 2.0f;

        // Delivery 
        DeliveryPoint currentDeliveryPoint;
        bool packageAttached;
        const float PICKUP_DISTANCE = 1.0f;
        const float DELIVERY_DISTANCE = 2.5f;

    };
} // namespace m1