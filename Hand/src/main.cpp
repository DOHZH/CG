
// Hand Example
// Author: Yi Kangrui <yikangrui@pku.edu.cn>

#define _CRT_SECURE_NO_WARNINGS

//#define DIFFUSE_TEXTURE_MAPPING

#include "gl_env.h"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>


#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#include <iostream>

#include "skeletal_mesh.h"

#include <glm\gtc\matrix_transform.hpp>

#include <stack>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

float start_time = 0.0f;
int status = 0;
//相机默认位置
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -50.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f,1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float fov=45.0f;//视角信息
float rotationX = 0.0f;//根据鼠标综合移动情况测算的旋转
float rotationY = 0.0f;
bool firstMouse = true;
float lastX = 400.0f, lastY = 400.0f;//设置初始位置
glm::mat3x3 result;

namespace SkeletalAnimation
{
	const char * vertex_shader_450 =
		"#version 450\n"
		"const int MAX_BONES = 100;\n"
		"uniform mat4 u_bone_transf[MAX_BONES];\n"
		"uniform mat4 u_mvp;\n"
		"layout(location = 0) in vec3 in_position;\n"
		"layout(location = 1) in vec2 in_texcoord;\n"
		"layout(location = 2) in vec3 in_normal;\n"
		"layout(location = 3) in ivec4 in_bone_index;\n"
		"layout(location = 4) in vec4 in_bone_weight;\n"
		"out vec2 pass_texcoord;\n"
		"void main() {\n"
		"    float adjust_factor = 0.0;\n"
		"    for (int i = 0; i < 4; i++) adjust_factor += in_bone_weight[i] * 0.25;\n"
		"    mat4 bone_transform = mat4(1.0);\n"
		"    if (adjust_factor > 1e-3) {\n"
		"        bone_transform -= bone_transform;\n"
		"        for (int i = 0; i < 4; i++)\n"
		"            bone_transform += u_bone_transf[in_bone_index[i]] * in_bone_weight[i] / adjust_factor;\n"
		"	 }\n"
		"    gl_Position = u_mvp * bone_transform * vec4(in_position, 1.0);\n"
		"    pass_texcoord = in_texcoord;\n"
		"}\n";

	const char* fragment_shader_450 =
		"#version 450\n"
		"uniform sampler2D u_diffuse;\n"
		"in vec2 pass_texcoord;\n"
		"out vec4 out_color;\n"
		"void main() {\n"
#ifdef DIFFUSE_TEXTURE_MAPPING
		"    out_color = vec4(texture(u_diffuse, pass_texcoord).xyz, 1.0);\n"
#else
		"    out_color = vec4(pass_texcoord, 0.0, 1.0);\n"
#endif
		"}\n";
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//相机上下左右前后控制
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * (-cameraUp);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * (-cameraUp);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += (cameraSpeed+0.3f) * (cameraFront);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= (cameraSpeed + 0.3f) * (cameraFront);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	//手势输入
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 1;
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 2;
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 3;
	}

	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 4;
	}

	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 5;
	}

	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 6;
	}

	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 7;
	}

	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 8;
	}

	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 9;
	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 10;
	}
	
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		start_time = clock() / double(CLOCKS_PER_SEC);
		status = 11;
	}

	//四元数相机从A点移动到B点的实现
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		status = 12;
		start_time = clock() / double(CLOCKS_PER_SEC);
	}

	//复位，令相机回到初始状态
	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
	{
		status = 13;
		start_time = clock() / double(CLOCKS_PER_SEC);
	}

	if (action == GLFW_RELEASE)
	{
		start_time = 0.0f;
		status = 0;
	}



}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int state_1 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	int state_2 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

	if (state == GLFW_PRESS)
	{
		float xoffset = xpos - lastX;
		lastX = xpos;
		float sensitivity = 0.002f;//设置灵敏度，防止对鼠标的操作反映过大
		xoffset *= sensitivity;
		rotationX += xoffset;

		glm::quat quaternionX;
		glm::quat result;

		result = glm::quat(cos(glm::radians(rotationX / 2)), 0.0f, sin(glm::radians(rotationX / 2)) * (-1),0.0f);
		cameraFront = result * cameraFront;
		cameraPos = result * cameraPos;
		cameraUp = result * cameraUp;
		
	}

	if (state_1 == GLFW_PRESS)
	{
		float yoffset = lastY - ypos;
		lastY = ypos;
		float sensitivity = 0.0005f;//设置灵敏度，防止对鼠标的操作反映过大
		yoffset *= sensitivity;
		rotationY -= yoffset;

		glm::quat quaternionY;
		glm::quat result;

		quaternionY = glm::quat(cos(glm::radians(rotationY / 2)), sin(glm::radians(rotationY / 2)) * 1, 0.0f, 0.0f);
		result = quaternionY;

		cameraFront = result * cameraFront;
		cameraPos = result * cameraPos;
		cameraUp = result * cameraUp;
	}

	if (state_2 == GLFW_PRESS)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		float sensitivity = 0.0005f;//设置灵敏度，防止对鼠标的操作反映过大
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		rotationX -= xoffset;
		rotationY += yoffset;
		
		glm::quat quaternionX;
		glm::quat quaternionY;
		glm::quat result;
		
		quaternionX = glm::quat(cos(glm::radians(rotationX / 2)), 0.0f, sin(glm::radians(rotationX / 2)) * 1, 0.0f);
		quaternionY = glm::quat(cos(glm::radians(rotationY / 2)), sin(glm::radians(rotationY / 2)) * 1, 0.0f, 0.0f);
		result = quaternionX * quaternionY;

		cameraFront = result * cameraFront;
		cameraPos = result * cameraPos;
		cameraUp = result * cameraUp;
	}


}
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 60.0f)
		fov -= yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 60.0f)
		fov = 60.0f;
}

//计算两个向量的旋转结果
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest, float time) {
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01)
			rotationAxis = cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f*time,
		rotationAxis.x * invs*time,
		rotationAxis.y * invs*time,
		rotationAxis.z * invs*time
	);

}
int main(int argc, char *argv[])
{
	GLFWwindow* window;
	GLuint vertex_shader, fragment_shader, program;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(800, 800, "OpenGL output", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);


	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &SkeletalAnimation::vertex_shader_450, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &SkeletalAnimation::fragment_shader_450, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	int linkStatus;
	if (glGetProgramiv(program, GL_LINK_STATUS, &linkStatus), linkStatus == GL_FALSE)
		std::cout << "Error occured in glLinkProgram()" << std::endl;

	SkeletalMesh::Scene & sr = SkeletalMesh::Scene::loadScene("Hand", "Hand.fbx");
	if (&sr == &SkeletalMesh::Scene::error)
		std::cout << "Error occured in loadMesh()" << std::endl;

	sr.setShaderInput(program, "in_position", "in_texcoord", "in_normal", "in_bone_index", "in_bone_weight");

	float passed_time = 0.0f;
	SkeletalMesh::SkeletonModifier modifier;

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		
		passed_time = clock() / double(CLOCKS_PER_SEC) - start_time;
		//相机
		glm::mat4 view_lookat;
		view_lookat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glm::highp_mat4 view_original = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));
		modifier["metacarpals"] = glm::rotate(view_original, glm::radians(0.0f), glm::fvec3(0.0, 0.0, 1.0));

		//没有键鼠输入时
		if (status == 0)
		{
			passed_time = clock() / double(CLOCKS_PER_SEC) - start_time;
			float period = 2.4f;
			float time_in_period = fmod(passed_time, period);
			// * angle: 0 -> PI/3 -> 0
			float thumb_angle = abs(time_in_period / (period * 0.5f) - 1.0f) * (M_PI / 3.0);
			//float thumb_angle = M_PI / 3.0;
			//float thumb_angle = (M_PI / 3.0);
			// * target = proximal phalange of the index
			// * rotation axis = (0, 0, 1)
			modifier["thumb_proximal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["index_proximal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(glm::fmat4(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}
		

		//数字1~9
		if (status == 1)
		{
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = 0.0f;
			modifier["index_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));


			thumb_angle = (M_PI / 3.0) * time_angle;
			modifier["thumb_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}

		if (status == 2)
		{
			
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = 0;
			modifier["index_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));


			thumb_angle = (M_PI / 3.0) * time_angle;
			modifier["thumb_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}

		if (status == 3)
		{
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = glm::radians(30.f) * time_angle;
			modifier["thumb_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = glm::radians(40.f) * time_angle;
			modifier["index_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = 0;
			modifier["thumb_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}

		if (status == 4)
		{
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = (M_PI / 3.0) * time_angle;
			modifier["thumb_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = 0;
			modifier["index_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}

		if (status == 5)
		{
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = 0;
			modifier["thumb_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["index_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}

		if (status == 6)
		{
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = 0;
			modifier["thumb_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = (M_PI/3.0) * time_angle;
			modifier["index_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}

		if (status == 7)
		{
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = glm::radians(45.f) * time_angle;
			modifier["thumb_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			thumb_angle = 0;
			modifier["thumb_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = glm::radians(60.f) * time_angle;
			modifier["index_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = 0;
			modifier["pinky_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
						
			modifier["index_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
						
			modifier["middle_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
						
			modifier["ring_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}

		if (status == 8)
		{
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);

			glm::highp_mat4 view = glm::rotate(glm::fmat4(), glm::radians(30.0f), glm::fvec3(0.0, 1.0, 0.0));
			//float metacarpals_angle = passed_time * (M_PI / 4.0);
			modifier["metacarpals"] = glm::rotate(view, 0.0f, glm::fvec3(0.3, 0.0, 0.2));

			glm::highp_mat4 view_1 = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));
			float thumb_angle = 0;
			modifier["thumb_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["index_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = glm::radians(60.0f) * time_angle;
			modifier["middle_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

		}

		if (status == 9)
		{	
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = 0;
			modifier["index_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = glm::radians(90.0f) * time_angle;
			modifier["index_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			thumb_angle = glm::radians(50.0f) * time_angle;
			modifier["index_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = glm::radians(90.0f) * time_angle;
			modifier["thumb_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
		}

		//抓握
		if (status == 10)
		{
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), glm::radians(90.0f), glm::fvec3(0.0, 1.0, 0.0));
			//float metacarpals_angle = passed_time * (M_PI/4.0);
			modifier["metacarpals"] = glm::rotate(view, 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view_1 = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = glm::radians(50.0f) * time_angle;
			modifier["index_proximal_phalange"] = glm::rotate(view_1, 0.0f, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view_1, glm::radians(60.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view_1, 0.0f, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view_1, glm::radians(60.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view_1, 0.0f, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view_1, glm::radians(60.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view_1, 0.0f, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view_1, glm::radians(60.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));

			glm::highp_mat4 spin_view = glm::rotate(glm::fmat4(), glm::radians(30.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_proximal_phalange"] = glm::rotate(spin_view, -glm::radians(20.0f) * time_angle, glm::fvec3(1.0, 0.0, 0.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view_1, glm::radians(10.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view_1, glm::radians(10.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view_1, -glm::radians(10.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));

		}

		//点赞
		if (status == 11)
		{
			glm::highp_mat4 view = glm::rotate(glm::fmat4(), glm::radians(90.0f), glm::fvec3(0.0, 1.0, 0.0));
			modifier["metacarpals"] = glm::rotate(view, glm::radians(90.0f), glm::fvec3(0.0, 0.0, 1.0));
			
			float period = 2.0f;
			float time_in_period = passed_time > period ? period : fmod(passed_time, period);
			float time_angle = abs(time_in_period / period);
			glm::highp_mat4 view_1 = glm::rotate(glm::fmat4(), 0.0f, glm::fvec3(0.0, 0.0, 1.0));

			float thumb_angle = -glm::radians(10.0f) * time_angle;
			modifier["thumb_proximal_phalange"] = glm::rotate(view_1, -glm::radians(30.0f) * time_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["thumb_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			thumb_angle = glm::radians(90.0f) * time_angle;
			modifier["index_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["index_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["middle_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["middle_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["ring_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["ring_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

			modifier["pinky_proximal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_intermediate_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_distal_phalange"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
			modifier["pinky_fingertip"] = glm::rotate(view_1, thumb_angle, glm::fvec3(0.0, 0.0, 1.0));

		}

		//四元数相机从A点到B点的移动
		if (status == 12)
		{
			//将本相机初始位置视作A点，B点为自定义点位。
			glm::vec3 start = glm::vec3(0.0f, 0.0f, -50.0f);
			glm::vec3 startFront = glm::vec3(0.0f, 0.0f, 1.0f);
			glm::vec3 startUp = glm::vec3(0.0f, 1.0f, 0.0f);
			float rotate = 10.0f;

			if (passed_time < 1.0f)
			{
				glm::quat rot1 = RotationBetweenVectors(start, glm::vec3(1.0f, 2.0f, 3.0f), passed_time);
				cameraPos = rot1 * start;
				cameraFront = rot1 * startFront;
				cameraUp = rot1 * startUp;
			}
			
		}

		//复位
		if (status == 13)
		{
			cameraPos = glm::vec3(0.0f, 0.0f, -50.0f);
			cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
			cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			deltaTime = 0.0f; // 当前帧与上一帧的时间差
			lastFrame = 0.0f; // 上一帧的时间
			fov = 45.0f;//视角信息
			rotationX = 0.0f;//根据鼠标综合移动情况测算的旋转
			rotationY = 0.0f;
			firstMouse = true;
			lastX = 400.0f;
			lastY = 400.0f;//设置初始位置

		}

		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glClearColor(0.5, 0.5, 0.5, 1.0);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		glm::mat4 projection = glm::perspective(glm::radians(fov), ratio, 0.1f, 100.0f);
		glm::fmat4 mvp = projection * view_lookat;
		glUniformMatrix4fv(glGetUniformLocation(program, "u_mvp"), 1, GL_FALSE, (const GLfloat*)&mvp);
		glUniform1i(glGetUniformLocation(program, "u_diffuse"), SCENE_RESOURCE_SHADER_DIFFUSE_CHANNEL);
		SkeletalMesh::Scene::SkeletonTransf bonesTransf;
		sr.getSkeletonTransform(bonesTransf, modifier);
		if (!bonesTransf.empty())
			glUniformMatrix4fv(glGetUniformLocation(program, "u_bone_transf"), bonesTransf.size(), GL_FALSE, (float *)bonesTransf.data());
		sr.render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	SkeletalMesh::Scene::unloadScene("Hand");

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}