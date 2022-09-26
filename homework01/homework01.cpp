// play with order of multiplication glm::mat4 MVP (79)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>

int main(int argc, char* argv[])
{
	// Initialise window
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1024, 1024, "Homework 1", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize extensions
	glewExperimental = true; // For some reason use max version
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// without it random object will be displyed, not closest
	glEnable(GL_DEPTH_TEST);
	// without it there is no transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load shaders
	char dir[1024];
   	getcwd( dir, sizeof( dir ) );
   	std::string cwd = dir;
	GLuint programTriangle1ID = LoadShaders((cwd + "/../homework01/Triangle1.vertexshader").c_str(),
								   (cwd + "/../homework01/Triangle1.fragmentshader").c_str());
	GLuint MatrixTriangle1ID = glGetUniformLocation(programTriangle1ID, "MVP");
	/*GLuint programTriangle2ID = LoadShaders((cwd + "/../homework01/Triangle2.vertexshader").c_str(),
								   (cwd + "/../homework01/Triangle2.fragmentshader").c_str());
	GLuint MatrixTriangle2ID = glGetUniformLocation(programTriangle1ID, "MVP");
	GLuint programPyramidID = LoadShaders((cwd + "/../homework01/Pyramid.vertexshader").c_str(),
								   (cwd + "/../homework01/Pyramid.fragmentshader").c_str());
	GLuint MatrixPyramidID = glGetUniformLocation(programTriangle1ID, "MVP");
	*/

	// Angle of view: 45, ratio: 1:1, display distance: 0.1 <-> 100
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Camera: pos(4, 3, 3), look(0, 0, 0), head(0, 1, 0)
	glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// Model
	glm::mat4 modelScaleMatrix = glm::scale(glm::vec3(1.0f, 0.8f, 1.25f));
	glm::vec3 modelRotationAxis(0.0f, 0.0f, 1.0f);
	glm::mat4 modelRotationMatrix = glm::rotate(45.0f, modelRotationAxis);
	glm::mat4 modelTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	glm::mat4 Model = modelTranslationMatrix * modelRotationMatrix * modelScaleMatrix;
	// ModelViewProjection
	glm::mat4 MVP = Projection * View * Model;
	//MVP = Projection * View * Model; // ???????????????????????????????????????????????????????????????????????????????????????????
	//MVP = Projection * Model * View;
	//MVP = View * Projection * Model;
	//MVP = View * Model * Projection;
	//MVP = Model * Projection * View;
	//MVP = Model * View * Projection;

	// create memory for vertices
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create models
	// Triangle1
	static const GLfloat g_vertex_triangle1_buffer_data[] = { 
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-3.0f,-1.0f,-2.0f,
		-3.0f,-1.0f, 0.0f,
		-3.0f, 1.0f, 0.0f
	};
	static const GLfloat g_color_triangle1_buffer_data[] = { 
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f
	};
	// Fill vertex buffer
	GLuint vertexTriangle1Buffer;
	glGenBuffers(1, &vertexTriangle1Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexTriangle1Buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_triangle1_buffer_data), g_vertex_triangle1_buffer_data, GL_STATIC_DRAW);
	// Fill color buffer
	GLuint colorTriangle1Buffer;
	glGenBuffers(1, &colorTriangle1Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorTriangle1Buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_triangle1_buffer_data), g_color_triangle1_buffer_data, GL_STATIC_DRAW);

	/*// Triangle2
	static const GLfloat g_vertex_triangle2_buffer_data[] = { 
		-5.0f,-1.0f,-1.0f,
		-5.0f,-1.0f, 1.0f,
		-5.0f, 1.0f, 1.0f
	};
	static const GLfloat g_color_triangle2_buffer_data[] = { 
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f
	};
	// Fill vertex buffer
	GLuint vertexTriangle2Buffer;
	glGenBuffers(1, &vertexTriangle2Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexTriangle2Buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_triangle2_buffer_data), g_vertex_triangle2_buffer_data, GL_STATIC_DRAW);
	// Fill color buffer
	GLuint colorTriangle2Buffer;
	glGenBuffers(1, &colorTriangle2Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorTriangle2Buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_triangle2_buffer_data), g_color_triangle2_buffer_data, GL_STATIC_DRAW);
	*/

	do{
		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable buffers with attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Draw Triangle1
		// Use shader
		glUseProgram(programTriangle1ID);
		glUniformMatrix4fv(MatrixTriangle1ID, 1, GL_FALSE, &MVP[0][0]);
		// Coordinates: ID: 0, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, vertexTriangle1Buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Colors: ID: 1, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, colorTriangle1Buffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Draw triangles
		glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

		/*// Draw Triangle2
		// Use shader
		glUseProgram(programTriangle2ID);
		glUniformMatrix4fv(MatrixTriangle2ID, 1, GL_FALSE, &MVP[0][0]);
		// Coordinates: ID: 0, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, vertexTriangle2Buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Colors: ID: 1, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, colorTriangle2Buffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Draw triangles
		glDrawArrays(GL_TRIANGLES, 0, 1 * 3);*/

		// Disable buffers with attributes
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup
	glDeleteBuffers(1, &vertexTriangle1Buffer);
	glDeleteBuffers(1, &colorTriangle1Buffer);
	glDeleteProgram(programTriangle1ID);
	/*glDeleteBuffers(1, &vertexTriangle2Buffer);
	glDeleteBuffers(1, &colorTriangle2Buffer);
	glDeleteProgram(programTriangle2ID);*/
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

