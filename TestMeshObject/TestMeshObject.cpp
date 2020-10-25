#include <iostream>

#include "texture_cube.h"
#include "camera.h"
#include "shader.h"

#include "MeshObject/HumanObject.h"

#define Height 0
#define Bust 1
#define Waist 2
#define Hip 3
#define ArmLengthR 4
#define ArmLengthL 5
#define LegLengthR 6
#define LegLengthL 7
#define ShoulderLength 8


float skybox_positions[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool isWindowed = true;
bool isKeyboardProcessed[1024] = { 0 };

// setting
const unsigned int SCR_WIDTH = 2048;
const unsigned int SCR_HEIGHT = 1024;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, -4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float TPoseFactor = 0.0f;

// Render mode
bool showWire = false;
bool showWirePressed = false, showWireCurrPressed = false;

bool show = false;
bool showPressed = false, showCurrPressed = false;

HumanObject *Human = NULL;

int main(int argc, char **argv)
{
	// Human->WriteObj("model/Result1.obj");
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello TestMeshObject!", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	/*** HUMAN ***/
	Shader humanShader("../usr/shaders/shader_texture.vs", "../usr/shaders/shader_texture.fs");

	Human = new HumanObject();
	// Human->LoadObj("model/Pose1.obj");

	Human->SetFemale();
	Human->LoadObj("model/Female/avatar_woman_new.obj");
	Human->LoadObjMtl("model/Female/avatar_woman.mtl");
	Human->LoadJoints("model/Female/avatar_woman_joints.Joint");
	Human->LoadLandmarks("model/Female/avatar_woman_landmarks_new.Landmark");

	/*
	Human->SetMale();
	Human->LoadObj("model/mick/source/Mick.obj");
	Human->LoadJoints("model/mick/source/Mick_joints.Joint");
	Human->LoadLandmarks("model/mick/source/Mick_landmarks.Landmark");
	*/



	// T 포즈로 변화
	// Human->SetTPose(1.0f);

	// 사이즈 변화
	Human->SetSize(Bust, 110.0);
	Human->SetSize(Waist, 100.0);
	Human->SetSize(Hip, 100.0);
	Human->SetSize(ArmLengthR, 100.0);
	/*
	*/

	std::cout << "\n\n======== Rendering Human" << std::endl;
	std::cout << "Vertices : " << Human->m_Vertices->size() << std::endl;
	std::cout << "Texels : " << Human->m_Texels->size() << std::endl;
	std::cout << "Normals : " << Human->m_Normals->size() << std::endl;
	std::cout << "Faces : " << Human->m_Faces->size() << std::endl;
	std::cout << "Vertex buffer : " << Human->m_VertBuf.size() << std::endl;
	std::cout << "Index buffer : " << Human->m_IndexBuf.size() << std::endl;

	std::cout << "\n\n======== Sizes" << std::endl;
	std::cout << "Height : " << (*Human->m_Landmarks)[Height]->m_Value << std::endl;
	std::cout << "Bust : " << (*Human->m_Landmarks)[Bust]->m_Value << std::endl;
	std::cout << "Waist : " << (*Human->m_Landmarks)[Waist]->m_Value << std::endl;
	std::cout << "Hip : " << (*Human->m_Landmarks)[Hip]->m_Value << std::endl;
	std::cout << "Arm Length R : " << (*Human->m_Landmarks)[ArmLengthR]->m_Value << std::endl;
	std::cout << "Arm Length L : " << (*Human->m_Landmarks)[ArmLengthL]->m_Value << std::endl;
	std::cout << "Leg Length R : " << (*Human->m_Landmarks)[LegLengthR]->m_Value << std::endl;
	std::cout << "Leg Length L : " << (*Human->m_Landmarks)[LegLengthL]->m_Value << std::endl;
	std::cout << "Shoulder Length : " << (*Human->m_Landmarks)[ShoulderLength]->m_Value << std::endl;


	GLuint humanVAO, humanVBO, humanEBO;
	glGenVertexArrays(1, &humanVAO);
	glGenBuffers(1, &humanVBO);
	glGenBuffers(1, &humanEBO);

	glBindVertexArray(humanVAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, humanEBO);
	std::vector<int> indexBuffer;
	std::map<std::string, std::vector<int>>::iterator it0;

	for (it0 = Human->m_IndexBuf.begin(); it0 != Human->m_IndexBuf.end(); it0++) {
		for (int i = 0; i < it0->second.size(); i++) {
			indexBuffer.push_back(it0->second[i]);
		}
	}

	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * Human->m_IndexBuf.size(), &Human->m_IndexBuf[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indexBuffer.size(), &indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, humanVBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Human->m_VertBuf.size(), &Human->m_VertBuf[0], GL_STATIC_DRAW);

	std::vector<float> renderBuffer;
	std::map<std::string, std::vector<float>>::iterator it;
	for (it = Human->m_VertBuf.begin(); it != Human->m_VertBuf.end(); it++) {
		for (int i = 0; i < it->second.size(); i++) {
			renderBuffer.push_back(it->second[i]);
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * renderBuffer.size(), &renderBuffer[0], GL_STATIC_DRAW);


	/*
	for (int i = 0; i < Human->m_IndexBuf.size(); i+=3) {
		std::cout << Human->m_IndexBuf[i] << " " << Human->m_IndexBuf[i + 1] << " " << Human->m_IndexBuf[i + 2] << std::endl;
	} 
	*/

	// Position
	GLuint humanPositionAttribute = glGetAttribLocation(humanShader.ID, "aPos");
	glVertexAttribPointer(humanPositionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, 0);
	glEnableVertexAttribArray(humanPositionAttribute);

	// Texture
	GLuint humanTextureAttribute = glGetAttribLocation(humanShader.ID, "aTexCoord");
	glVertexAttribPointer(humanTextureAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void *)(sizeof(float) * 3));
	glEnableVertexAttribArray(humanTextureAttribute);

	// Normal
	GLuint humanNormalAttribute = glGetAttribLocation(humanShader.ID, "aNormal");
	glVertexAttribPointer(humanNormalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void *)(sizeof(float) * 5));
	glEnableVertexAttribArray(humanNormalAttribute);

	// Color
	GLuint humanColorAttribute = glGetAttribLocation(humanShader.ID, "aColor");
	glVertexAttribPointer(humanColorAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void *)(sizeof(float) * 8));
	glEnableVertexAttribArray(humanColorAttribute);
	glBindVertexArray(0);

	humanShader.use();
	unsigned int humanTextureId;

	if (!Human->m_Texels->empty()) {
		humanShader.setInt("isTexture", 0);
		// humanShader.setVec3("Color", glm::vec3(0.3, 0.3, 0.3));
	}
	else  {
		humanShader.setInt("isTexture", 1);
		humanTextureId = glGetUniformLocation(humanShader.ID, "texture");

		glGenTextures(1, &humanTextureId);
		glBindTexture(GL_TEXTURE_2D, humanTextureId);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(humanTextureId, 0);

		// Human->RenderTexture();
	}
	/*************/

	/*** JOINT ***/
	Shader jointShader("../usr/shaders/shader_default.vs", "../usr/shaders/shader_default.fs");

	float joints[Joint_Num * 3];
	int j = 0;
	for (int i = 0; i < Joint_Num; i++) {
		joints[j] = (*Human->m_Skeleton->m_Joints)[i]->m_Coord->x;
		joints[j + 1] = (*Human->m_Skeleton->m_Joints)[i]->m_Coord->y;
		joints[j + 2] = (*Human->m_Skeleton->m_Joints)[i]->m_Coord->z;
		j += 3;
	}

	int bones[Bone_Num * 2];
	j = 0;
	for (int i = 0; i < Human->m_Skeleton->m_Bones->size(); i++) {
		bones[j] = (*Human->m_Skeleton->m_Bones)[i]->m_UpperJoint->m_Idx;
		bones[j + 1] = (*Human->m_Skeleton->m_Bones)[i]->m_LowerJoint->m_Idx;
		j += 2;
	}

	GLuint jointVAO, jointVBO, jointEBO;
	glGenVertexArrays(1, &jointVAO);
	glGenBuffers(1, &jointVBO);
	glGenBuffers(1, &jointEBO);

	glBindVertexArray(jointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, jointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(joints), &joints, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jointEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bones) , &bones, GL_STATIC_DRAW);

	GLuint jointPosition = glGetAttribLocation(jointShader.ID, "aPos");
	glVertexAttribPointer(jointPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(jointPosition);

	glBindVertexArray(0);

	jointShader.use();
	jointShader.setVec3("Color", glm::vec3(1, 0, 0));
	/*************/

	/*** SKYBOX ***/
	Shader skyboxShader("../usr/shaders/shader_skybox.vs", "../usr/shaders/shader_skybox.fs");

	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_positions), skybox_positions, GL_STATIC_DRAW);

	GLuint skyboxPositionAttribute = glGetAttribLocation(skyboxShader.ID, "aPos");
	glVertexAttribPointer(skyboxPositionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(skyboxPositionAttribute);
	glBindVertexArray(0);

	std::vector<std::string> skyboxFaces{
	"../usr/resources/right.jpg",
	"../usr/resources/left.jpg",
	"../usr/resources/top.jpg",
	"../usr/resources/bottom.jpg",
	"../usr/resources/front.jpg",
	"../usr/resources/back.jpg"
	};

	skyboxShader.use();
	int skyboxDayID = glGetUniformLocation(skyboxShader.ID, "skyboxDay");

	glActiveTexture(GL_TEXTURE0);
	stbi_set_flip_vertically_on_load(false);
	CubemapTexture skyboxTexture = CubemapTexture(skyboxFaces);
	glUniform1i(skyboxDayID, 0);
	/**************/


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//												GAME LOOP											 //
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window)) {
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/*** Render Human ***/
		humanShader.use();
		
		// Lighting
		humanShader.setVec3("CameraPos", camera.Position);
		humanShader.setVec3("LightPos", glm::vec3(0, 5, 0));
		humanShader.setVec3("LightColor", glm::vec3(1, 1, 1));

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		humanShader.setMat4("projection", projection);
		glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
		humanShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.01f));
		model = glm::translate(model, -(glm::vec3(4.82843, -2.41421, -0.2002) - camera.Position));
		humanShader.setMat4("model", model);

		glBindVertexArray(humanVAO);

		glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, humanTextureId);

		if (showWire)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

		if (show) {
			// glDrawElements(GL_TRIANGLES, 120, GL_UNSIGNED_INT, 0);
			glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, 0);
		}
		// glDrawArrays(GL_TRIANGLES, 0, 120);
		glDrawArrays(GL_TRIANGLES, 0, renderBuffer.size() / 3);

		// Human->Render();


		glBindVertexArray(0);
		/********************/

		/*** Render Joint ***/
		jointShader.use();
		jointShader.setMat4("projection", projection);
		jointShader.setMat4("view", view);
		jointShader.setMat4("model", model);

		glPointSize(10.0f);
		glBindVertexArray(jointVAO);
		glDrawArrays(GL_POINTS, 0, Joint_Num * 3);
		glDrawElements(GL_LINES, sizeof(bones) / sizeof(int), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		/********************/

		/*** Render Skybox ***/
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		skyboxShader.setMat4("projection", projection);
		glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", skyboxView);

		glBindVertexArray(skyboxVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		/*********************/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Human->WriteObj("model/Female/Tpose_test.obj");

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// TODO : make camera movable (WASD) & increase or decrease dayFactor(press O: increase, press P: decrease)

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		if (TPoseFactor >= 1.0f)
			TPoseFactor = 1.0f;
		else
			TPoseFactor += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		if (TPoseFactor <= 0.0f)
			TPoseFactor = 0.0f;
		else
			TPoseFactor -= 0.1f;
	}


	showWireCurrPressed = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
	if (!showWirePressed && showWireCurrPressed) {
		showWire = !showWire;
	}
	showWirePressed = showWireCurrPressed;

	showCurrPressed = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
	if (!showPressed && showCurrPressed) {
		show = !show;
	}
	showPressed = showCurrPressed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// TODO: calculate how much cursor have moved, rotate camera proportional to the value, using ProcessMouseMovement.
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	camera.ProcessMouseMovement(xpos - lastX, lastY - ypos);
	lastX = xpos;
	lastY = ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
