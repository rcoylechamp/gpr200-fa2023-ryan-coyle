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

struct Light {
	ew::Vec3 Position = ew::Vec3(0, 0, 0);
	ew::Vec3 Color = ew::Vec3(1, 1, 1);
	ew::Vec3 LightColor = ew::Vec3(1.0f, 1.0f, 1.0f);
	
} light;
const int MAX_LIGHT = 4;
Light lights[MAX_LIGHT];
int numOfLights = 1;
bool lightsOrbit = true;
float orbitRadius = 2.0f;

//main.cpp
struct Material {
	float ambientK = 0.5; //Ambient coefficient (0-1)
	float diffuseK = 0.5; //Diffuse coefficient (0-1)
	float specular = 0.5; //Specular coefficient (0-1)
	float shininess = 2; //Shininess

} material;


void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

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

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	//UNLIT SHADER
	ew::Shader unlitShader("assets/unLit.vert", "assets/unLit.frag");

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	resetCamera(camera,cameraController);
	
	ew::Mesh lightMesh = ew::Mesh(ew::createSphere(0.5f, 16));
	ew::Transform lightTrans[MAX_LIGHT];
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		for (int i = 0; i < numOfLights; i++) {
			shader.setVec3("_Lights[" + std::to_string(i) + "].position", lightTrans[i].position);
			shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].Color);
		}

		shader.setVec3("_LightColor", light.LightColor);
		shader.setVec3("_CameraPos", camera.position);

		shader.setFloat("_Ambient", material.ambientK);
		shader.setFloat("_Diffuse", material.diffuseK);
		shader.setFloat("_Specular", material.specular);
		shader.setFloat("_Shininess", material.shininess);

		shader.setInt("_NumLights", numOfLights);

		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < numOfLights; i++) {
			lightTrans[i].scale = 0.5f;
			if (lightsOrbit) {
				lightTrans[i].position.x = cos(i * (2 * ew::PI / 4) + time) * orbitRadius;
				lightTrans[i].position.z = -sin(i * (2 * ew::PI / 4) + time) * orbitRadius;
			}

		

			unlitShader.setMat4("_Model", lightTrans[i].getModelMatrix());
			unlitShader.setVec3("_Color", lights[i].Color);
			lightMesh.draw();
		}


		//Render UI
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


			ImGui::DragInt("Num Lights", &numOfLights, 1.0f, 0.0f, MAX_LIGHT);

			ImGui::Checkbox("Orbit Lights", &lightsOrbit);
			ImGui::DragFloat("Orbit Radius", &orbitRadius, 0.1f, 0.5f);

			for (int i = 0; i < numOfLights; i++) {

				ImGui::PushID(i);

				if (ImGui::CollapsingHeader("Light")) {
					ImGui::DragFloat3("Position", &lightTrans[i].position.x, 0.1f);
					ImGui::ColorEdit3("Color", &lights[i].Color.x, 0.1f);
				}
				ImGui::PopID();
			}

			if (ImGui::CollapsingHeader("Material")) {
				ImGui::ColorEdit3("Light Color", &light.LightColor.x);
				ImGui::DragFloat("AmbientK", &material.ambientK, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("DiffuseK", &material.diffuseK, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("SpecularK", &material.specular, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("ShininessK", &material.shininess, 0.1f, 2.0f, 100.0f);
			}	


			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
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


