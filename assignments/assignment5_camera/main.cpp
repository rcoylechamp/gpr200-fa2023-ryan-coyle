#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <rc/transformations.h>
#include <rc/shader.h>
#include <rc/camera.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void moveCamera(GLFWwindow* window, rc::Camera* Camera, rc::CameraControls* Controls);


//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}
	rc::CameraControls myCameraControls;
	rc::Camera myCamera;
	myCamera.position = ew::Vec3(0, 0, 5);
	myCamera.target = ew::Vec3(0, 0, 0);
	myCamera.fov = 60.0;
	myCamera.aspectRatio = (float)(SCREEN_WIDTH) / (float)(SCREEN_HEIGHT);
	myCamera.orthographic = false;
	myCamera.orthoSize = 6.0;
	myCamera.nearPlane = 0.1;
	myCamera.farPlane = 100;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set uniforms
		shader.use();
	
		moveCamera(window, &myCamera, &myCameraControls);
		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			shader.setMat4("_View", myCamera.ViewMatrix());
			shader.setMat4("_Projection", myCamera.ProjectionMatrix());
			cubeMesh.draw();
		}

		

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::Text("Camera");

			ImGui::DragFloat3("Position", &myCamera.position.x, 0.05f);
			ImGui::DragFloat3("Target", &myCamera.target.x, 0.05f);

			ImGui::Checkbox("Orthographic", &myCamera.orthographic);
			if (myCamera.orthographic) {
				ImGui::DragFloat("OrthoSize", &myCamera.orthoSize, 0.5f);
			}
			else if (!myCamera.orthographic) {
				ImGui::DragFloat("FOV", &myCamera.fov, 0.05f);
			}
			ImGui::DragFloat("Near Plane", &myCamera.nearPlane, 0.5f);
			ImGui::DragFloat("Far Plane", &myCamera.farPlane, 0.5f);


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
}

void moveCamera(GLFWwindow* window, rc::Camera* myCamera, rc::CameraControls* myControls) {
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		myControls->firstMouse = true;
		return;
	}
	//GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Get screen mouse position this frame
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);


	//If we just started right clicking, set prevMouse values to current position.
	//This prevents a bug where the camera moves as soon as we click.

	if (myControls->firstMouse) {
		myControls->firstMouse = false;
		myControls->prevMouseX = mouseX;
		myControls->prevMouseY = mouseY;
	}

//TODO: Get mouse position delta for this frame
//TODO: Add to yaw and pitch
//TODO: Clamp pitch between -89 and 89 degrees
//radians?
	//0.1 for mouse sensitivity
	//ew::Vec3 yaw = ew::DEG2RAD;
	int mouseSensitivity = 0.1;


	//Remember previous mouse position
	myControls->prevMouseX = mouseX;
	myControls->prevMouseY = mouseY;

	//ew::Vec3 forward = ;

}