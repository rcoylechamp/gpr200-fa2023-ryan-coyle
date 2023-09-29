#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <rc/shader.h>

//can use namespace like below, but easier to see if using "rc::"
//using namespace rc; 



unsigned int createVAO(float* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float vertices[12] = {
	//x    y    z
	-0.5, -0.5, 0.0, //Bottom left
	 0.5, -0.5, 0.0, //Bottom right
	 0.5,  0.5, 0.0, //Top right
	-0.5,  0.5, 0.0,//Top left
};

unsigned int indices[6] = {
2 , 3 , 1, //Triangle 1
4 , 3 , 1  //Triangle 2
};




float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;
bool showImGUIDemoWindow = true;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}


	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
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

	std::string vertexShaderSource = rc::loadShaderSourceFromFile("assets/vertexShader.vert");
	std::string fragmentShaderSource = rc::loadShaderSourceFromFile("assets/fragmentShader.frag");

	rc::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();

	
	unsigned int vao = createVAO(vertices, 4, indices, 6);

	//glUseProgram(shader1);
	glBindVertexArray(vao);


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		//Set uniforms
		//glUniform3f(glGetUniformLocation(shader, "_Color"), triangleColor[0], triangleColor[1], triangleColor[2]);
		//glUniform1f(glGetUniformLocation(shader,"_Brightness"), triangleBrightness);
		shader.setFloat("_Brightness", triangleBrightness);
		shader.setVec3("_Color", triangleColor[0], triangleColor[1], triangleColor[2]);

		//Wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		
		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::ColorEdit3("Color", triangleColor);
			ImGui::SliderFloat("Brightness", &triangleBrightness, 0.0f, 1.0f);
			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}


			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}




unsigned int createVAO(float* vertexData, int numVertices, unsigned int* indicesData, int numIndices){


	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertexData, GL_STATIC_DRAW);

	//in createVAO()...
	glBindVertexArray(vao);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (const void*)0);
	glEnableVertexAttribArray(0);

	return vao;
}
//if window size changes, everything is re centered
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

