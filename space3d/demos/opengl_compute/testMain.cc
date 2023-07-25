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
#include "renderer.h"

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	unsigned int win_width = 900;
	unsigned int win_height = 675;
	// Create a GLFWwindow object that we can use for GLFW's functions    
	GLFWwindow* window = glfwCreateWindow(win_width, win_height, "compute_shader", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
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
    std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(win_width, win_height);
    //Renderer* renderer = new Renderer(win_width, win_height);
	// Clear the colorbuffer    
	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
	// //
	// GLuint VertexArrayID;
	// glGenVertexArrays(1, &VertexArrayID);
	// glBindVertexArray(VertexArrayID);

	// // Create and compile our GLSL program from the shaders
	// // GLuint programID = LoadShaders("colorTri.vert", "colorTri.frag");
    // auto   path_vert = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "demos/opengl/colorTri.vert";
    // auto   path_vertString = path_vert.string();
    // auto   path_frag  = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "demos/opengl/colorTri.frag";
    // auto   path_fragString = path_frag.string();
    // GLuint programID       = LoadShaders(path_vertString.c_str(), path_fragString.c_str());


	// static const GLfloat g_vertex_buffer_data[] = {
	// 	-1.0f, -1.0f, 0.0f,
	// 	1.0f, -1.0f, 0.0f,
	// 	0.0f,  1.0f, 0.0f,
	// };

	// GLuint vertexbuffer;
	// glGenBuffers(1, &vertexbuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Game loop    
	while (!glfwWindowShouldClose(window))
	{
		// Render
		//glClear(GL_COLOR_BUFFER_BIT);

		renderer->render();

		// Swap the screen buffers    
		glfwSwapBuffers(window);
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions    
		glfwPollEvents();

	}
	glfwTerminate();

	//system("PAUSE");
	return 0;
}