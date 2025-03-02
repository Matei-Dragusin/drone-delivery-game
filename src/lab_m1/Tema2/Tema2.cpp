#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <random>

using namespace std;
using namespace m1;

Tema2::Tema2()
{
    camera = nullptr;
    minimapCamera = nullptr;
    minimapSize = 200.0f;
    dronePosition = glm::vec3(0);
    droneRotation = glm::vec3(0);
    propellerRotation = 0;
    droneForward = glm::vec3(0, 0, -1);
}

Tema2::~Tema2()
{
    delete camera;
    delete minimapCamera;
}

void Tema2::Init()
{
    // Initialize cameras
    camera = new implemented::Camera();
    camera->distanceToTarget = 5.0f;

    minimapCamera = new implemented::Camera();
    minimapCamera->position = glm::vec3(0, MINIMAP_CAMERA_HEIGHT, 0);
    minimapCamera->forward = glm::vec3(0, -1, 0); // Looking down
    minimapCamera->up = glm::vec3(0, 0, -1);      // Adjusted up vector
    minimapCamera->right = glm::vec3(1, 0, 0);    // Right vector stays the same

    // Initialize positions
    dronePosition = glm::vec3(0, 2.0f, 0);
    droneRotation = glm::vec3(0);
    propellerRotation = 0;
    droneForward = glm::vec3(0, 0, -1);

    // Initialize projections
    fov = 60;
    zNear = 0.01f;
    zFar = 200.0f;
    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);

    float mapSize = 50.0f; // Size of the visible map area in minimap
    minimapProjectionMatrix = glm::ortho(-mapSize / 2, mapSize / 2, -mapSize / 2, mapSize / 2, 0.1f, 1000.0f);

    polygonMode = GL_FILL;

    // Create meshes
    CreateDroneMeshes();
    CreateGroundMesh(100, 100);
    CreateMinimapMarkerMesh();
    CreateMinimapTreeMarkerMesh();
    CreateTreeMeshes();
    GenerateRandomTrees(50);
    CreatePlatformMesh();
    CreatePackageMesh();
    currentPackage.isActive = false;
    packageAttached = false;
    currentDeliveryPoint.isActive = false;
    SpawnPackageOnRandomPlatform();

    // Create and initialize shaders
    Shader *shader = new Shader("TerrainShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
}

void Tema2::CreateMinimapMarkerMesh()
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-0.5f, 0, -0.5f), glm::vec3(0, 0, 0)), // negru
        VertexFormat(glm::vec3(0.5f, 0, -0.5f), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(-0.5f, 0, 0.5f), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(0.5f, 0, 0.5f), glm::vec3(0, 0, 0))};

    std::vector<unsigned int> indices = {
        0, 1, 2,
        1, 3, 2};

    Mesh *minimapMarker = new Mesh("minimap_marker");
    minimapMarker->InitFromData(vertices, indices);
    minimapMarker->SetDrawMode(GL_TRIANGLES);
    meshes[minimapMarker->GetMeshID()] = minimapMarker;
}

void Tema2::CreateMinimapTreeMarkerMesh()
{
    // Use a lighter green for better visibility
    glm::vec3 treeColor = glm::vec3(0.0f, 0.8f, 0.0f);

    std::vector<VertexFormat> vertices = {
        // A larger square for better visibility
        VertexFormat(glm::vec3(-1.0f, 0, -1.0f), treeColor),
        VertexFormat(glm::vec3(1.0f, 0, -1.0f), treeColor),
        VertexFormat(glm::vec3(-1.0f, 0, 1.0f), treeColor),
        VertexFormat(glm::vec3(1.0f, 0, 1.0f), treeColor)};

    std::vector<unsigned int> indices = {
        0, 1, 2,
        1, 3, 2};

    Mesh *treeMarker = new Mesh("minimap_tree_marker");
    treeMarker->InitFromData(vertices, indices);
    meshes[treeMarker->GetMeshID()] = treeMarker;
}

void Tema2::CreateDroneMeshes()
{
    // Colors
    glm::vec3 grey = glm::vec3(0.5f);
    glm::vec3 black = glm::vec3(0);

    {
        // Create the drone body (X shape from two parallelepipeds)
        vector<VertexFormat> vertices = {
            // First arm
            VertexFormat(glm::vec3(-0.5f, 0, -0.5f), grey),
            VertexFormat(glm::vec3(0.5f, 0, 0.5f), grey),
            VertexFormat(glm::vec3(-0.5f, 0.1f, -0.5f), grey),
            VertexFormat(glm::vec3(0.5f, 0.1f, 0.5f), grey),
            VertexFormat(glm::vec3(-0.4f, 0, -0.6f), grey),
            VertexFormat(glm::vec3(0.6f, 0, 0.4f), grey),
            VertexFormat(glm::vec3(-0.4f, 0.1f, -0.6f), grey),
            VertexFormat(glm::vec3(0.6f, 0.1f, 0.4f), grey),

            // Second arm
            VertexFormat(glm::vec3(-0.5f, 0, 0.5f), grey),
            VertexFormat(glm::vec3(0.5f, 0, -0.5f), grey),
            VertexFormat(glm::vec3(-0.5f, 0.1f, 0.5f), grey),
            VertexFormat(glm::vec3(0.5f, 0.1f, -0.5f), grey),
            VertexFormat(glm::vec3(-0.4f, 0, 0.6f), grey),
            VertexFormat(glm::vec3(0.6f, 0, -0.4f), grey),
            VertexFormat(glm::vec3(-0.4f, 0.1f, 0.6f), grey),
            VertexFormat(glm::vec3(0.6f, 0.1f, -0.4f), grey),
        };

        vector<unsigned int> indices = {
            // First arm
            0, 1, 2, 1, 3, 2,
            0, 4, 2, 4, 6, 2,
            1, 5, 3, 5, 7, 3,
            4, 5, 6, 5, 7, 6,

            // Second arm
            8, 9, 10, 9, 11, 10,
            8, 12, 10, 12, 14, 10,
            9, 13, 11, 13, 15, 11,
            12, 13, 14, 13, 15, 14};

        meshes["drone_body"] = new Mesh("drone_body");
        meshes["drone_body"]->InitFromData(vertices, indices);
    }

    {
        // Create cube for propeller support
        vector<VertexFormat> vertices = {
            // Bottom
            VertexFormat(glm::vec3(-0.1f, 0, -0.1f), grey),
            VertexFormat(glm::vec3(0.1f, 0, -0.1f), grey),
            VertexFormat(glm::vec3(-0.1f, 0, 0.1f), grey),
            VertexFormat(glm::vec3(0.1f, 0, 0.1f), grey),
            // Top
            VertexFormat(glm::vec3(-0.1f, 0.2f, -0.1f), grey),
            VertexFormat(glm::vec3(0.1f, 0.2f, -0.1f), grey),
            VertexFormat(glm::vec3(-0.1f, 0.2f, 0.1f), grey),
            VertexFormat(glm::vec3(0.1f, 0.2f, 0.1f), grey),
        };

        vector<unsigned int> indices = {
            // Bottom
            0, 1, 2, 1, 3, 2,
            // Top
            4, 5, 6, 5, 7, 6,
            // Front
            0, 1, 4, 1, 5, 4,
            // Back
            2, 3, 6, 3, 7, 6,
            // Left
            0, 2, 4, 2, 6, 4,
            // Right
            1, 3, 5, 3, 7, 5};

        meshes["propeller_support"] = new Mesh("propeller_support");
        meshes["propeller_support"]->InitFromData(vertices, indices);
    }

    {
        // Create propeller mesh
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-0.2f, 0.2f, -0.02f), black),
            VertexFormat(glm::vec3(0.2f, 0.2f, -0.02f), black),
            VertexFormat(glm::vec3(-0.2f, 0.25f, -0.02f), black),
            VertexFormat(glm::vec3(0.2f, 0.25f, -0.02f), black),
            VertexFormat(glm::vec3(-0.2f, 0.2f, 0.02f), black),
            VertexFormat(glm::vec3(0.2f, 0.2f, 0.02f), black),
            VertexFormat(glm::vec3(-0.2f, 0.25f, 0.02f), black),
            VertexFormat(glm::vec3(0.2f, 0.25f, 0.02f), black),
        };

        vector<unsigned int> indices = {
            // Top
            2, 3, 6, 3, 7, 6,
            // Bottom
            0, 1, 4, 1, 5, 4,
            // Front
            0, 1, 2, 1, 3, 2,
            // Back
            4, 5, 6, 5, 7, 6,
            // Left
            0, 2, 4, 2, 6, 4,
            // Right
            1, 3, 5, 3, 7, 5};

        meshes["propeller"] = new Mesh("propeller");
        meshes["propeller"]->InitFromData(vertices, indices);
    }
}

void Tema2::RenderMeshWithCustomCamera(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix,
                                       implemented::Camera *customCamera, const glm::mat4 &customProjection)
{
    if (!mesh || !shader || !shader->program || !customCamera)
        return;

    shader->Use();

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(customCamera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(customProjection));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}
void Tema2::RenderMainScene(float deltaTimeSeconds)
{
    // Update propeller rotation
    propellerRotation += deltaTimeSeconds * 10.0f;

    // Render drone body
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, dronePosition);
    modelMatrix = glm::rotate(modelMatrix, droneRotation.y, glm::vec3(0, 1, 0));
    RenderMesh(meshes["drone_body"], shaders["VertexColor"], modelMatrix);

    // Positions for propellers
    glm::vec3 propellerPositions[] = {
        glm::vec3(-0.5f, 0, -0.5f),
        glm::vec3(0.5f, 0, 0.5f),
        glm::vec3(-0.5f, 0, 0.5f),
        glm::vec3(0.5f, 0, -0.5f)};

    // Render propellers
    for (const auto &pos : propellerPositions)
    {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition);
        modelMatrix = glm::rotate(modelMatrix, droneRotation.y, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, pos);
        RenderMesh(meshes["propeller_support"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::rotate(modelMatrix, propellerRotation, glm::vec3(0, 1, 0));
        RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);
    }

    // Render ground
    modelMatrix = glm::mat4(1);
    RenderMesh(meshes["ground"], shaders["TerrainShader"], modelMatrix);

    // Render trees
    for (size_t i = 0; i < treePositions.size(); i++)
    {
        float scale = treeScales[i];
        glm::vec3 position = treePositions[i];

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
        RenderMesh(meshes["tree_trunk"], shaders["VertexColor"], modelMatrix);

        glm::mat4 leavesMatrix = modelMatrix;
        leavesMatrix = glm::translate(leavesMatrix, glm::vec3(0, 0.8f, 0));
        leavesMatrix = glm::scale(leavesMatrix, glm::vec3(1.0f, 0.8f, 1.0f));
        RenderMesh(meshes["tree_leaves"], shaders["VertexColor"], leavesMatrix);

        leavesMatrix = glm::translate(leavesMatrix, glm::vec3(0, 0.5f, 0));
        leavesMatrix = glm::scale(leavesMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
        RenderMesh(meshes["tree_leaves"], shaders["VertexColor"], leavesMatrix);
    }

    // Render platforms
    for (size_t i = 0; i < platforms.size(); i++)
    {
        const auto &platform = platforms[i];
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, platform.position);
        modelMatrix = glm::scale(modelMatrix, platform.scale);

        // Choose the correct mesh based on the platform type
        if (currentDeliveryPoint.isActive && i == currentDeliveryPoint.platformIndex)
        {
            RenderMesh(meshes["delivery_platform"], shaders["VertexColor"], modelMatrix);
        }
        else
        {
            RenderMesh(meshes["platform"], shaders["VertexColor"], modelMatrix);
        }
    }
}

void Tema2::FrameStart()
{
    // Clears the color buffer and depth buffer
    glClearColor(0.529f, 0.808f, 0.922f, 1); // Light blue sky color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

float Tema2::GetTerrainHeight(float x, float z)
{
    float frequency = 0.3f;
    glm::vec2 noiseCoord = glm::vec2(x, z) * frequency;

    // Implement the same noise function as in the shader
    auto noise2D = [](glm::vec2 st)
    {
        glm::vec2 i = glm::floor(st);
        glm::vec2 f = glm::fract(st);

        float a = glm::fract(sin(glm::dot(i, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
        float b = glm::fract(sin(glm::dot(i + glm::vec2(1.0f, 0.0f), glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
        float c = glm::fract(sin(glm::dot(i + glm::vec2(0.0f, 1.0f), glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
        float d = glm::fract(sin(glm::dot(i + glm::vec2(1.0f, 1.0f), glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);

        glm::vec2 u = f * f * (glm::vec2(3.0f) - 2.0f * f);

        return glm::mix(a, b, u.x) +
               (c - a) * u.y * (1.0f - u.x) +
               (d - b) * u.x * u.y;
    };

    // Use the same noise formula as in the vertex shader
    float n = noise2D(noiseCoord) * 0.5f +
              noise2D(noiseCoord * 2.0f) * 0.25f +
              noise2D(noiseCoord * 4.0f) * 0.125f;

    return n * 2.0f; // Same scale factor as in the vertex shader
}

void Tema2::Update(float deltaTimeSeconds)
{
    // Clears the color buffer and depth buffer
    glClearColor(0.529f, 0.808f, 0.922f, 1); // Light blue sky color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw for main scene
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    // Update main camera
    glm::mat4 droneRotationMatrix = glm::rotate(glm::mat4(1.0f), droneRotation.y, glm::vec3(0, 1, 0));
    glm::vec3 rotatedForward = glm::normalize(glm::vec3(droneRotationMatrix * glm::vec4(0, 0, -1, 0)));
    glm::vec3 targetPosition = dronePosition + glm::vec3(0, 1.0f, 0);
    glm::vec3 cameraOffset = -rotatedForward * camera->distanceToTarget;
    cameraOffset.y = 2.0f;

    camera->position = targetPosition + cameraOffset;
    camera->forward = glm::normalize(targetPosition - camera->position);
    camera->right = glm::normalize(glm::cross(camera->forward, glm::vec3(0, 1, 0)));
    camera->up = glm::cross(camera->right, camera->forward);

    // Render main scene
    RenderMainScene(deltaTimeSeconds);

    UpdatePackageLogic();

    RenderPackage(deltaTimeSeconds);
    RenderDirectionArrow();

    // Update and render minimap
    if (minimapCamera != nullptr)
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::ivec2 resolution = window->GetResolution();
        glViewport(resolution.x - minimapSize, resolution.y - minimapSize, minimapSize, minimapSize);

        minimapCamera->position = glm::vec3(dronePosition.x, MINIMAP_CAMERA_HEIGHT, dronePosition.z);

        // Render ground
        modelMatrix = glm::mat4(1);
        RenderMeshWithCustomCamera(meshes["ground"], shaders["TerrainShader"], modelMatrix,
                                   minimapCamera, minimapProjectionMatrix);

        // Render platforms with increased visibility
        for (const auto &platform : platforms)
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix,
                                         glm::vec3(platform.position.x, 1.5f, platform.position.z));
            modelMatrix = glm::scale(modelMatrix,
                                     glm::vec3(platform.scale.x, 0.2f, platform.scale.z));
            RenderMeshWithCustomCamera(meshes["platform"], shaders["VertexColor"], modelMatrix,
                                       minimapCamera, minimapProjectionMatrix);
        }

        // Render trees as small dots
        for (const auto &treePos : treePositions)
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix,
                                         glm::vec3(treePos.x, 1.0f, treePos.z)); // Raise above the terrain
            modelMatrix = glm::scale(modelMatrix,
                                     glm::vec3(0.3f)); // Make markers larger for visibility
            RenderMeshWithCustomCamera(
                meshes["minimap_tree_marker"],
                shaders["VertexColor"], // Use the color shader
                modelMatrix,
                minimapCamera,
                minimapProjectionMatrix);
        }

        // Render drone marker (large black dot)
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(dronePosition.x, 10.0f, dronePosition.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f)); // Enlarge the drone marker
        RenderMeshWithCustomCamera(meshes["minimap_marker"], shaders["VertexColor"], modelMatrix,
                                   minimapCamera, minimapProjectionMatrix);

        // Restore viewport
        glViewport(0, 0, resolution.x, resolution.y);
    }
}

void Tema2::CreateGroundMesh(int rows, int cols)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float cellWidth = 1.0f;
    float cellHeight = 1.0f;

    // Generate vertices
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            float x = j * cellWidth - (cols * cellWidth / 2);
            float z = i * cellHeight - (rows * cellHeight / 2);

            glm::vec3 position(x, 0, z);
            glm::vec3 normal(0, 1, 0);
            glm::vec3 color(0.5f);

            vertices.push_back(VertexFormat(position, color, normal));
        }
    }

    // Generate indices
    for (int i = 0; i < rows - 1; i++)
    {
        for (int j = 0; j < cols - 1; j++)
        {
            unsigned int topLeft = i * cols + j;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (i + 1) * cols + j;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh *ground = new Mesh("ground");
    ground->InitFromData(vertices, indices);
    meshes["ground"] = ground;
}

void Tema2::CreateTreeMeshes()
{
    // Colors
    glm::vec3 trunkColor = glm::vec3(0.36f, 0.25f, 0.20f);  // Brown
    glm::vec3 leavesColor = glm::vec3(0.13f, 0.55f, 0.13f); // Dark green

    // Create the tree trunk (cylinder)
    {
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        const int numSegments = 12;
        const float height = 1.0f;
        const float radius = 0.15f;

        // Top and bottom center vertices
        vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), trunkColor));
        vertices.push_back(VertexFormat(glm::vec3(0, height, 0), trunkColor));

        // Generate vertices for the sides
        for (int i = 0; i < numSegments; i++)
        {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = radius * cos(angle);
            float z = radius * sin(angle);

            // Bottom vertex
            vertices.push_back(VertexFormat(glm::vec3(x, 0, z), trunkColor));
            // Top vertex
            vertices.push_back(VertexFormat(glm::vec3(x, height, z), trunkColor));

            // Indices for the side faces
            int baseIndex = 2 + i * 2;
            int nextBaseIndex = 2 + ((i + 1) % numSegments) * 2;

            // Bottom triangle
            indices.push_back(baseIndex);
            indices.push_back(nextBaseIndex);
            indices.push_back(0);

            // Top triangle
            indices.push_back(baseIndex + 1);
            indices.push_back(nextBaseIndex + 1);
            indices.push_back(1);

            // Side face - two triangles
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 1);
            indices.push_back(nextBaseIndex);

            indices.push_back(nextBaseIndex);
            indices.push_back(baseIndex + 1);
            indices.push_back(nextBaseIndex + 1);
        }

        meshes["tree_trunk"] = new Mesh("tree_trunk");
        meshes["tree_trunk"]->InitFromData(vertices, indices);
    }

    // Create the cone for leaves
    {
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        const int numSegments = 12;
        const float height = 1.5f;
        const float radius = 0.8f;

        // Cone tip
        vertices.push_back(VertexFormat(glm::vec3(0, height, 0), leavesColor));
        // Center of the base
        vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), leavesColor));

        // Generate vertices for the base
        for (int i = 0; i < numSegments; i++)
        {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = radius * cos(angle);
            float z = radius * sin(angle);

            vertices.push_back(VertexFormat(glm::vec3(x, 0, z), leavesColor));

            // Indices for the side face
            indices.push_back(0); // tip
            indices.push_back(2 + i);
            indices.push_back(2 + (i + 1) % numSegments);

            // Indices for the base
            indices.push_back(1); // center of the base
            indices.push_back(2 + i);
            indices.push_back(2 + (i + 1) % numSegments);
        }

        meshes["tree_leaves"] = new Mesh("tree_leaves");
        meshes["tree_leaves"]->InitFromData(vertices, indices);
    }
}

bool Tema2::CheckTreeOverlap(const glm::vec3 &position, float radius)
{
    for (const auto &existingPos : treePositions)
    {
        float distance = glm::length(position - existingPos);
        if (distance < TREE_MIN_DISTANCE)
        {
            return true;
        }
    }
    return false;
}

void Tema2::CreatePlatformMesh()
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 redColor = glm::vec3(1, 0, 0);
    glm::vec3 blueColor = glm::vec3(0, 0, 1);

    // Create two meshes: one red and one blue
    auto createPlatformWithColor = [&](const std::string &name, const glm::vec3 &color)
    {
        vertices = {
            // Front face
            VertexFormat(glm::vec3(-0.5f, -0.5f, 0.5f), color),
            VertexFormat(glm::vec3(0.5f, -0.5f, 0.5f), color),
            VertexFormat(glm::vec3(0.5f, 0.5f, 0.5f), color),
            VertexFormat(glm::vec3(-0.5f, 0.5f, 0.5f), color),

            // Back face
            VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), color),
            VertexFormat(glm::vec3(-0.5f, 0.5f, -0.5f), color),
            VertexFormat(glm::vec3(0.5f, 0.5f, -0.5f), color),
            VertexFormat(glm::vec3(0.5f, -0.5f, -0.5f), color),

            // Top face
            VertexFormat(glm::vec3(-0.5f, 0.5f, -0.5f), color),
            VertexFormat(glm::vec3(-0.5f, 0.5f, 0.5f), color),
            VertexFormat(glm::vec3(0.5f, 0.5f, 0.5f), color),
            VertexFormat(glm::vec3(0.5f, 0.5f, -0.5f), color),

            // Bottom face
            VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), color),
            VertexFormat(glm::vec3(0.5f, -0.5f, -0.5f), color),
            VertexFormat(glm::vec3(0.5f, -0.5f, 0.5f), color),
            VertexFormat(glm::vec3(-0.5f, -0.5f, 0.5f), color),

            // Right face
            VertexFormat(glm::vec3(0.5f, -0.5f, -0.5f), color),
            VertexFormat(glm::vec3(0.5f, 0.5f, -0.5f), color),
            VertexFormat(glm::vec3(0.5f, 0.5f, 0.5f), color),
            VertexFormat(glm::vec3(0.5f, -0.5f, 0.5f), color),

            // Left face
            VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), color),
            VertexFormat(glm::vec3(-0.5f, -0.5f, 0.5f), color),
            VertexFormat(glm::vec3(-0.5f, 0.5f, 0.5f), color),
            VertexFormat(glm::vec3(-0.5f, 0.5f, -0.5f), color)};

        indices = {
            0, 1, 2, 0, 2, 3,       // Front
            4, 5, 6, 4, 6, 7,       // Back
            8, 9, 10, 8, 10, 11,    // Top
            12, 13, 14, 12, 14, 15, // Bottom
            16, 17, 18, 16, 18, 19, // Right
            20, 21, 22, 20, 22, 23  // Left
        };

        Mesh *platform = new Mesh(name);
        platform->InitFromData(vertices, indices);
        meshes[platform->GetMeshID()] = platform;
    };

    createPlatformWithColor("platform", redColor);
    createPlatformWithColor("delivery_platform", blueColor);

    // Generate 10 random platforms
    platforms.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistrib(-MAP_SIZE / 2, MAP_SIZE / 2);
    std::uniform_real_distribution<float> heightDistrib(3.0f, 5.0f); // Height between 3 and 5 units

    const float platformSize = 3.0f; // Smaller size for platforms
    const int maxAttempts = 100;
    int platformsCreated = 0;

    while (platformsCreated < 10 && maxAttempts > 0)
    {
        float x = posDistrib(gen);
        float z = posDistrib(gen);
        float height = heightDistrib(gen);

        glm::vec3 position(x, height / 2, z); // Divide height by 2 because scale will double the height

        if (!CheckPlatformCollisions(position, platformSize))
        {
            platforms.push_back({
                position,
                glm::vec3(platformSize, height, platformSize) // Vertical scale is height
            });
            platformsCreated++;
        }
    }
}

bool Tema2::CheckPlatformCollisions(const glm::vec3 &position, float platformSize)
{
    // Check overlap with trees
    float minDistance = platformSize / 2 + TREE_MIN_DISTANCE;

    for (const auto &treePos : treePositions)
    {
        float distance = glm::length(glm::vec2(position.x - treePos.x, position.z - treePos.z));
        if (distance < minDistance)
        {
            return true; // Overlap exists
        }
    }

    // Check overlap with other platforms
    for (const auto &platform : platforms)
    {
        float distance = glm::length(glm::vec2(position.x - platform.position.x,
                                               position.z - platform.position.z));
        if (distance < platformSize + 2.0f)
        { // Add space between platforms
            return true;
        }
    }

    return false;
}

void Tema2::GenerateRandomTrees(int numTrees)
{
    treePositions.clear();
    treeScales.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistrib(-MAP_SIZE / 2, MAP_SIZE / 2);
    std::uniform_real_distribution<float> scaleDistrib(0.8f, 1.2f);

    for (int i = 0; i < numTrees; i++)
    {
        glm::vec3 position;
        bool validPosition = false;
        int attempts = 0;

        while (!validPosition && attempts < 100)
        {
            float x = posDistrib(gen);
            float z = posDistrib(gen);
            float y = GetTerrainHeight(x, z);
            position = glm::vec3(x, y, z);
            validPosition = !CheckTreeOverlap(position, TREE_MIN_DISTANCE);
            attempts++;
        }

        if (validPosition)
        {
            treePositions.push_back(position);
            treeScales.push_back(scaleDistrib(gen));
        }
    }
}

void Tema2::CreatePackageMesh()
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Color for the package - a light brown
    glm::vec3 packageColor = glm::vec3(0.8f, 0.6f, 0.4f);

    // Dimensions of the package
    float width = 0.4f;
    float height = 0.4f;
    float depth = 0.4f;

    // Vertices for a cube
    vertices = {
        // Front face
        VertexFormat(glm::vec3(-width, -height, depth), packageColor),
        VertexFormat(glm::vec3(width, -height, depth), packageColor),
        VertexFormat(glm::vec3(width, height, depth), packageColor),
        VertexFormat(glm::vec3(-width, height, depth), packageColor),

        // Back face
        VertexFormat(glm::vec3(-width, -height, -depth), packageColor),
        VertexFormat(glm::vec3(-width, height, -depth), packageColor),
        VertexFormat(glm::vec3(width, height, -depth), packageColor),
        VertexFormat(glm::vec3(width, -height, -depth), packageColor),
    };

    // Indices for all faces of the cube
    indices = {
        0, 1, 2, 0, 2, 3, // front
        4, 5, 6, 4, 6, 7, // back
        3, 2, 6, 3, 6, 5, // top
        0, 4, 7, 0, 7, 1, // bottom
        0, 3, 5, 0, 5, 4, // left
        1, 7, 6, 1, 6, 2  // right
    };

    Mesh *package = new Mesh("package");
    package->InitFromData(vertices, indices);
    meshes[package->GetMeshID()] = package;

    // Create the mesh for the arrow
    vertices.clear();
    indices.clear();

    glm::vec3 arrowColor = glm::vec3(0.0f, 1.0f, 0.0f);

    float arrowLength = 0.3f;
    float arrowWidth = 0.1f;
    float arrowHeight = 0.0f; // Keep the arrow flat on the Y axis

    // The tip of the arrow will be in front (negative Z)
    vertices = {
        // Tip of the arrow
        VertexFormat(glm::vec3(0, arrowHeight, -arrowLength), arrowColor),

        // Base of the triangle
        VertexFormat(glm::vec3(-arrowWidth, arrowHeight, arrowLength), arrowColor),
        VertexFormat(glm::vec3(arrowWidth, arrowHeight, arrowLength), arrowColor),
    };

    // Indices for a simple triangle
    indices = {
        0, 1, 2 // A single triangle
    };

    Mesh *arrow = new Mesh("direction_arrow");
    arrow->InitFromData(vertices, indices);
    arrow->SetDrawMode(GL_TRIANGLES);
    meshes[arrow->GetMeshID()] = arrow;
}

void Tema2::SpawnPackageOnRandomPlatform()
{
    if (platforms.empty())
        return;

    int platformIndex = rand() % platforms.size();
    const Platform &platform = platforms[platformIndex];

    // Position the package exactly on the platform
    // Use the height of the package (0.4f) to position it correctly
    currentPackage.position = glm::vec3(
        platform.position.x,
        platform.position.y + platform.scale.y / 2 + 0.2f, // Adjust to 0.2f to be flush with the platform
        platform.position.z);

    currentPackage.isActive = true;
    currentPackage.platformIndex = platformIndex;
}

void Tema2::RenderPackage(float deltaTimeSeconds)
{
    if (!currentPackage.isActive)
        return;

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, currentPackage.position);

    if (packageAttached)
    {
        // Rotate the package to align with the drone
        modelMatrix = glm::rotate(modelMatrix, droneRotation.y, glm::vec3(0, 1, 0));
    }

    RenderMesh(meshes["package"], shaders["VertexColor"], modelMatrix);
}

void Tema2::RenderDirectionArrow()
{
    if (!currentPackage.isActive && !packageAttached)
        return;

    glm::vec3 targetPosition;
    if (!packageAttached)
    {
        // Point to the package when not attached
        targetPosition = currentPackage.position;
    }
    else
    {
        // Point to the delivery point when the package is attached
        targetPosition = platforms[currentDeliveryPoint.platformIndex].position;
    }

    // Calculate direction in the XZ plane
    glm::vec2 dronePos2D = glm::vec2(dronePosition.x, dronePosition.z);
    glm::vec2 targetPos2D = glm::vec2(targetPosition.x, targetPosition.z);
    glm::vec2 direction2D = targetPos2D - dronePos2D;

    float targetAngle = atan2(direction2D.x, direction2D.y);

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, ARROW_HEIGHT, 0));
    modelMatrix = glm::rotate(modelMatrix, targetAngle + glm::radians(180.0f), glm::vec3(0, 1, 0));

    RenderMesh(meshes["direction_arrow"], shaders["VertexColor"], modelMatrix);
}

void Tema2::SpawnDeliveryPoint()
{
    if (platforms.empty())
        return;

    // Choose a platform different from the one the package is on
    int platformIndex;
    do
    {
        platformIndex = rand() % platforms.size();
    } while (platformIndex == currentPackage.platformIndex);

    currentDeliveryPoint.platformIndex = platformIndex;
    currentDeliveryPoint.isActive = true;
}

void Tema2::UpdatePackageLogic()
{
    if (!packageAttached)
    {
        if (currentPackage.isActive)
        {
            // Calculate the 3D distance between the drone and the package
            float distance = glm::length(dronePosition - currentPackage.position);

            // Check if the drone is close enough to the package
            if (distance < PICKUP_DISTANCE)
            {
                // Check if the new position of the package would be valid
                glm::vec3 potentialPackagePos = dronePosition + glm::vec3(0, -0.7f, 0);
                bool canAttach = true;

                // Check collision with platforms for the potential position
                for (const auto &platform : platforms)
                {
                    float halfWidth = platform.scale.x / 2;
                    float halfHeight = platform.scale.y / 2;
                    float halfDepth = platform.scale.z / 2;
                    float packageWidth = 0.4f;
                    float packageHeight = 0.4f;

                    if (potentialPackagePos.x + packageWidth / 2 >= platform.position.x - halfWidth &&
                        potentialPackagePos.x - packageWidth / 2 <= platform.position.x + halfWidth &&
                        potentialPackagePos.z + packageWidth / 2 >= platform.position.z - halfDepth &&
                        potentialPackagePos.z - packageWidth / 2 <= platform.position.z + halfDepth &&
                        potentialPackagePos.y + packageHeight >= platform.position.y - halfHeight &&
                        potentialPackagePos.y <= platform.position.y + halfHeight)
                    {
                        canAttach = false;
                        break;
                    }
                }

                // Attach the package only if the position is valid
                if (canAttach)
                {
                    packageAttached = true;
                    SpawnDeliveryPoint();
                }
            }
        }
    }

    if (packageAttached)
    {
        // Update the position of the package relative to the drone
        currentPackage.position = dronePosition + glm::vec3(0, -0.7f, 0);

        // Check if we are at the delivery point
        if (currentDeliveryPoint.isActive)
        {
            const Platform &deliveryPlatform = platforms[currentDeliveryPoint.platformIndex];
            glm::vec3 platformTopCenter = deliveryPlatform.position +
                                          glm::vec3(0, deliveryPlatform.scale.y / 2 + 0.2f, 0);

            float deliveryDistance = glm::length(currentPackage.position - platformTopCenter);

            if (deliveryDistance < DELIVERY_DISTANCE)
            {
                // Successful delivery
                packageAttached = false;
                currentPackage.isActive = false;
                currentDeliveryPoint.isActive = false;
                SpawnPackageOnRandomPlatform();
            }
        }
    }
}

void Tema2::FrameEnd()
{
}

void Tema2::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float moveSpeed = 2.0f;
    float rotateSpeed = 2.0f;
    float minHeightAboveTerrain = 0.5f;

    // Drone dimensions
    float droneWidth = 1.5f;
    float droneHeight = 0.5f;

    // Package dimensions
    float packageWidth = 0.4f;
    float packageHeight = 0.4f;

    // Tree dimensions
    float treeTrunkRadius = 0.15f;
    float treeHeight = 2.5f;
    float treeCrownRadius = 0.8f;

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), droneRotation.y, glm::vec3(0, 1, 0));
    glm::vec3 localForward = glm::vec3(glm::normalize(rotationMatrix * glm::vec4(0, 0, -1, 0)));
    glm::vec3 localRight = glm::vec3(glm::normalize(rotationMatrix * glm::vec4(1, 0, 0, 0)));
    glm::vec3 localUp = glm::vec3(0, 1, 0);

    // Process each movement direction separately
    glm::vec3 totalMovement(0);

    // Helper function for collision checking
    auto checkCollision = [&](const glm::vec3 &testDronePos, const glm::vec3 &testPackagePos) -> bool
    {
        // Check platforms
        for (const auto &platform : platforms)
        {
            float halfWidth = platform.scale.x / 2;
            float halfHeight = platform.scale.y / 2;
            float halfDepth = platform.scale.z / 2;

            bool droneCollision = (testDronePos.x + droneWidth / 2 >= platform.position.x - halfWidth &&
                                   testDronePos.x - droneWidth / 2 <= platform.position.x + halfWidth &&
                                   testDronePos.z + droneWidth / 2 >= platform.position.z - halfDepth &&
                                   testDronePos.z - droneWidth / 2 <= platform.position.z + halfDepth &&
                                   testDronePos.y + droneHeight >= platform.position.y - halfHeight &&
                                   testDronePos.y <= platform.position.y + halfHeight);

            bool packageCollision = packageAttached && (testPackagePos.x + packageWidth / 2 >= platform.position.x - halfWidth &&
                                                        testPackagePos.x - packageWidth / 2 <= platform.position.x + halfWidth &&
                                                        testPackagePos.z + packageWidth / 2 >= platform.position.z - halfDepth &&
                                                        testPackagePos.z - packageWidth / 2 <= platform.position.z + halfDepth &&
                                                        testPackagePos.y + packageHeight >= platform.position.y - halfHeight &&
                                                        testPackagePos.y <= platform.position.y + halfHeight);

            if (droneCollision || packageCollision)
                return true;
        }

        // Check trees
        for (size_t i = 0; i < treePositions.size(); i++)
        {
            const glm::vec3 &treePos = treePositions[i];
            float treeScale = treeScales[i];

            float scaledTrunkRadius = treeTrunkRadius * treeScale;
            float scaledTreeHeight = treeHeight * treeScale;
            float scaledCrownRadius = treeCrownRadius * treeScale * 1.5f;

            float droneHorizontalDist = glm::length(
                glm::vec2(testDronePos.x - treePos.x, testDronePos.z - treePos.z));
            float packageHorizontalDist = packageAttached ? glm::length(glm::vec2(testPackagePos.x - treePos.x, testPackagePos.z - treePos.z)) : 999999.0f;

            float crownBaseHeight = treePos.y + scaledTreeHeight * 0.4f;

            // Collision with trunk
            if ((droneHorizontalDist < scaledTrunkRadius + droneWidth / 2 &&
                 testDronePos.y <= treePos.y + scaledTreeHeight &&
                 testDronePos.y + droneHeight >= treePos.y) ||
                (packageAttached &&
                 packageHorizontalDist < scaledTrunkRadius + packageWidth / 2 &&
                 testPackagePos.y <= treePos.y + scaledTreeHeight &&
                 testPackagePos.y + packageHeight >= treePos.y))
            {
                return true;
            }

            // Collision with crown
            if ((droneHorizontalDist < scaledCrownRadius &&
                 testDronePos.y + droneHeight >= crownBaseHeight &&
                 testDronePos.y <= treePos.y + scaledTreeHeight) ||
                (packageAttached &&
                 packageHorizontalDist < scaledCrownRadius &&
                 testPackagePos.y + packageHeight >= crownBaseHeight &&
                 testPackagePos.y <= treePos.y + scaledTreeHeight))
            {
                float droneRelativeHeight = (testDronePos.y - crownBaseHeight) / (scaledTreeHeight - crownBaseHeight);
                float packageRelativeHeight = packageAttached ? (testPackagePos.y - crownBaseHeight) / (scaledTreeHeight - crownBaseHeight) : 1.0f;

                float droneCrownRadius = scaledCrownRadius * (1.0f - droneRelativeHeight);
                float packageCrownRadius = scaledCrownRadius * (1.0f - packageRelativeHeight);

                if (droneHorizontalDist < droneCrownRadius + droneWidth / 2 ||
                    (packageAttached && packageHorizontalDist < packageCrownRadius + packageWidth / 2))
                {
                    return true;
                }
            }
        }

        return false;
    };

    // Process each movement direction individually
    glm::vec3 movement(0);
    float step = deltaTime * moveSpeed;

    // Forward/Backward
    if (window->KeyHold(GLFW_KEY_W) || window->KeyHold(GLFW_KEY_S))
    {
        glm::vec3 forwardMove = window->KeyHold(GLFW_KEY_W) ? localForward * step : -localForward * step;
        glm::vec3 testDronePos = dronePosition + forwardMove;
        glm::vec3 testPackagePos = packageAttached ? testDronePos + glm::vec3(0, -0.7f, 0) : dronePosition;

        if (!checkCollision(testDronePos, testPackagePos))
        {
            movement += forwardMove;
        }
    }

    // Right/Left
    if (window->KeyHold(GLFW_KEY_D) || window->KeyHold(GLFW_KEY_A))
    {
        glm::vec3 rightMove = window->KeyHold(GLFW_KEY_D) ? localRight * step : -localRight * step;
        glm::vec3 testDronePos = dronePosition + rightMove;
        glm::vec3 testPackagePos = packageAttached ? testDronePos + glm::vec3(0, -0.7f, 0) : dronePosition;

        if (!checkCollision(testDronePos, testPackagePos))
        {
            movement += rightMove;
        }
    }

    // Up/Down
    if (window->KeyHold(GLFW_KEY_E) || window->KeyHold(GLFW_KEY_Q))
    {
        glm::vec3 upMove = window->KeyHold(GLFW_KEY_E) ? localUp * step : -localUp * step;
        glm::vec3 testDronePos = dronePosition + upMove;
        glm::vec3 testPackagePos = packageAttached ? testDronePos + glm::vec3(0, -0.7f, 0) : dronePosition;

        if (!checkCollision(testDronePos, testPackagePos))
        {
            movement += upMove;
        }
    }

    // Apply movement
    glm::vec3 newDronePosition = dronePosition + movement;

    // Check minimum height above terrain
    float terrainHeight = GetTerrainHeight(newDronePosition.x, newDronePosition.z);
    if (newDronePosition.y < terrainHeight + minHeightAboveTerrain)
    {
        newDronePosition.y = terrainHeight + minHeightAboveTerrain;
    }

    // Update positions
    dronePosition = newDronePosition;
    if (packageAttached)
    {
        currentPackage.position = dronePosition + glm::vec3(0, -0.7f, 0);
    }

    // Rotation around local Y axis
    if (window->KeyHold(GLFW_KEY_LEFT))
    {
        droneRotation.y += deltaTime * rotateSpeed;
    }
    if (window->KeyHold(GLFW_KEY_RIGHT))
    {
        droneRotation.y -= deltaTime * rotateSpeed;
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
}

void Tema2::OnKeyRelease(int key, int mods)
{
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Rotate drone based on mouse movement when left button is held
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensitivity = 0.002f;
        droneRotation.y += deltaX * sensitivity;
    }
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
}