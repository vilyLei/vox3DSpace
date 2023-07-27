#include <iostream>
#include <filesystem>
//#include <windows.h>
// GLEW
#define GLEW_STATIC    
#include <include/GL/glew.h>

// GLFW
#include <include/glfw3.h>
//
#include "shader.h"

// Is called whenever a key is pressed/released via GLFW    
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
int main()
{

	// Init GLFW    
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	
	// Create a GLFWwindow object that we can use for GLFW's functions    
	GLFWwindow* window = glfwCreateWindow(900, 650, "BinShader-ColorTriangle", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

    //GLFWimage icons[1];
    //glfwSetWindowIcon(window, 1, icons);
	// 
	//o_assert_dbg(1);
	glfwMakeContextCurrent(window);
	// Set the required callback functions    
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions    
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers    
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions    
	//glViewport(0, 0, 900, 650);

	// Clear the colorbuffer    
	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
	//
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	// GLuint programID = LoadShaders("colorTri.vert", "colorTri.frag");
    auto   path_vert = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "demos/opengl_binShader/colorTri.vert.spv";
    auto   path_vertString = path_vert.string();
    auto   path_frag  = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "demos/opengl_binShader/colorTri.frag.spv";
    auto   path_fragString = path_frag.string();
    //GLuint programID       = LoadShaders(path_vertString, path_fragString);
    GLuint programID = LoadBinShaders(path_vertString, path_fragString);


	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Game loop    
	while (!glfwWindowShouldClose(window))
	{
		// Render
		glClear(GL_COLOR_BUFFER_BIT);
		// render
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);


		// Swap the screen buffers    
		glfwSwapBuffers(window);
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions    
		glfwPollEvents();

	}
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);
	// Terminate GLFW, clearing any resources allocated by GLFW.    
	glfwTerminate();

	//system("PAUSE");
	return EXIT_SUCCESS;
}