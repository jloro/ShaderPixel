/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/17 15:36:27 by jloro             #+#    #+#             */
/*   Updated: 2019/06/20 16:39:20 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.hpp"
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Camera.hpp"
#include "Model.hpp"

void processInput(GLFWwindow *window, Camera & cam)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.Move(Forward);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.Move(Backward);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.Move(Right);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.Move(Left);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cam.Move(Up);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cam.Move(Down);
}

void mouse_callback(GLFWwindow* window, Camera & cam)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	cam.LookAround(xpos, ypos);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	} 
	glViewport(0, 0, 800, 600);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	std::vector<const char *>	t{"shaders/vertex.glsl", "shaders/fragment.glsl"};
	std::vector<GLenum>			type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	double tmpx, tmpy;
	glfwGetCursorPos(window, &tmpx, &tmpy);
	Shader	test(t, type);
	Camera	cam(800, 600, tmpx, tmpy);

	std::string path= "nanosuit/nanosuit.obj";
	Model crysis(path.c_str());

	while (!glfwWindowShouldClose(window))
	{
		processInput(window, cam);
		cam.UpdateFrame();
		mouse_callback(window, cam);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		test.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		test.setMat4("model", model);
		test.setMat4("view", cam.GetMatView());
		test.setMat4("projection", cam.GetMatProj());

		crysis.Draw(test);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
