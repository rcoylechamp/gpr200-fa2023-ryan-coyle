#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;


float waveFreq = 5;
float waveSpeed = 1.0f;
float waveStrength = .2f;

int scale = 50;

struct Light {
    ew::Vec3 Position = ew::Vec3(0, 0, 0);
    ew::Vec3 Color = ew::Vec3(1, 1, 1);
    ew::Vec3 LightColor = ew::Vec3(1.0f, 1.0f, 1.0f);

} light;

const int MAX_LIGHT = 4;
Light lights[MAX_LIGHT];
int numOfLights = 1;

//main.cpp
struct Material {
    float ambientK = 0.5; //Ambient coefficient (0-1)
    float diffuseK = 0.5; //Diffuse coefficient (0-1)
    float specular = 0.65; //Specular coefficient (0-1)
    float shininess = 2; //Shininess
} material;


int main() {
    printf("Initializing...");
    if (!glfwInit()) {
        printf("GLFW failed to init!");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
    if (window == NULL) {
        printf("GLFW failed to create window");
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("GLAD Failed to load GL headers");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");

    unsigned int waterTexture = ew::loadTexture("assets/ocean_v2.jpg", GL_REPEAT, GL_LINEAR);
    

    ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");

    //water mesh BIG
    ew::Mesh waterMesh(ew::createPlane(100, 100, 50));

    ew::Transform waterPlane;
    waterPlane.position = ew::Vec3(0, -20.0, 0);

    resetCamera(camera, cameraController);
    
    ew::Transform lightTrans[MAX_LIGHT];

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glDisable(GL_CULL_FACE);

        float time = (float)glfwGetTime();
        float deltaTime = time - prevTime;
        prevTime = time;

        camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
        cameraController.Move(window, &camera, deltaTime);

        //RENDER
        glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind textures to texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterTexture);

        

        shader.use();

        float _ReflectAmt = 1;
        float _surfaceStrength = .32f;
        float _surfaceSpeed = 0.07f;

        shader.setFloat("uTime", glfwGetTime());
        shader.setFloat("uWaveSpeed", waveSpeed);
        shader.setFloat("uWaveFreq", waveFreq);
        shader.setFloat("uWaveStrength", waveStrength);
        shader.setFloat("_ReflectAmt", _ReflectAmt);
        shader.setFloat("_SurfaceStrength", _surfaceStrength);
        shader.setFloat("_SurfaceSpeed", _surfaceSpeed);

       
        shader.setInt("_WaterTexture", 0);

        shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
        shader.setMat4("_Model", waterPlane.getModelMatrix());
        waterMesh.draw();

        for (int i = 0; i < numOfLights; i++) {
            shader.setVec3("_Lights[" + std::to_string(i) + "].position", lightTrans[i].position);
            shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].Color);
        }

        // Render point lights
        shader.setInt("_NumLights", numOfLights);
        shader.setVec3("_CameraPos", camera.position);

        shader.setFloat("_Ambient", material.ambientK);
        shader.setFloat("_Diffuse", material.diffuseK);
        shader.setFloat("_Specular", material.specular);
        shader.setFloat("_Shininess", material.shininess);

       

        unlitShader.use();
        unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

        // Render UI
        {
            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");
            if (ImGui::CollapsingHeader("Camera")) {
                ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
                ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
                ImGui::Checkbox("Orthographic", &camera.orthographic);
                if (camera.orthographic) {
                    ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
                }
                else {
                    ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
                }
                ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
                ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
                ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
                ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
                if (ImGui::Button("Reset")) {
                    resetCamera(camera, cameraController);
                }
            }

            ImGui::ColorEdit3("BG color", &bgColor.x);
            ImGui::DragInt("Light Intensity", &numOfLights, 1.0f, 0.0f, MAX_LIGHT);
           
            for (int i = 0; i < numOfLights; i++) {

                ImGui::PushID(i);

                if (ImGui::CollapsingHeader("Light")) {
                    ImGui::DragFloat3("Position", &lightTrans[i].position.x, 0.1f);
                    ImGui::ColorEdit3("Color", &lights[i].Color.x, 0.1f);
                }
                ImGui::PopID();
            }

                ImGui::DragFloat("SpecularK", &material.specular, 0.01f, 0.0f, 1.0f);
            
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window);
    }
    printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
    camera.position = ew::Vec3(0, 0, 5);
    camera.target = ew::Vec3(0);
    camera.fov = 60.0f;
    camera.orthoHeight = 6.0f;
    camera.nearPlane = 0.1f;
    camera.farPlane = 100.0f;
    camera.orthographic = false;

    cameraController.yaw = 0.0f;
    cameraController.pitch = 0.0f;
}