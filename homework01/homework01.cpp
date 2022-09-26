// play with order of multiplication glm::mat4 MVP (79)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>


long long current_ms() {
	using namespace std::chrono;
	return static_cast<long long>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	// edge case for transparent
	glDepthFunc(GL_LEQUAL);

	// Load shaders
	char dir[1024];
   	getcwd( dir, sizeof( dir ) );
   	std::string cwd = dir;
	GLuint programTriangle1ID = LoadShaders((cwd + "/../homework01/Triangle1.vertexshader").c_str(),
								   (cwd + "/../homework01/Triangle1.fragmentshader").c_str());
	GLuint MatrixTriangle1ID = glGetUniformLocation(programTriangle1ID, "MVP");
	GLuint programTriangle2ID = LoadShaders((cwd + "/../homework01/Triangle2.vertexshader").c_str(),
								   (cwd + "/../homework01/Triangle2.fragmentshader").c_str());
	GLuint MatrixTriangle2ID = glGetUniformLocation(programTriangle1ID, "MVP");
	GLuint programPyramidID = LoadShaders((cwd + "/../homework01/Pyramid.vertexshader").c_str(),
								   (cwd + "/../homework01/Pyramid.fragmentshader").c_str());
	GLuint MatrixPyramidID = glGetUniformLocation(programTriangle1ID, "MVP");

	// create memory for vertices
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create models
	// Triangles
	static const GLfloat g_vertex_triangles_buffer_data[] = {
		 0.000f,  0.000f,  0.000f,
		-1.000f, -1.000f, -1.000f,
		 1.000f, -1.000f, -1.000f,
		 1.000f,  1.000f, -1.000f,
		-1.000f,  1.000f, -1.000f,
		-1.000f, -1.000f, -1.000f,
	};
	static const GLfloat g_color_triangles_buffer_data[] = {
		 0.583f,  0.771f,  0.014f,
		 0.609f,  0.115f,  0.436f,
		 0.327f,  0.483f,  0.844f,
		 0.200f,  0.771f,  0.600f,
		 0.300f,  0.115f,  0.700f,
		 0.400f,  0.483f,  0.200f,
	};
	// Fill vertex buffer
	GLuint vertexTrianglesBuffer;
	glGenBuffers(1, &vertexTrianglesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexTrianglesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_triangles_buffer_data), g_vertex_triangles_buffer_data, GL_STATIC_DRAW);
	// Fill color buffer
	GLuint colorTrianglesBuffer;
	glGenBuffers(1, &colorTrianglesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorTrianglesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_triangles_buffer_data), g_color_triangles_buffer_data, GL_STATIC_DRAW);

	// Pyramid
	static const GLfloat g_vertex_pyramid_buffer_data[] = {
		 0.0f,  0.0f,  0.0f, //
		 2.0f,  2.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f, //
		-2.0f,  2.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f, //
		 2.0f,  2.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,
		 2.0f,  2.0f,  0.0f, //
		 0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  1.0f,  0.0f, //
		-2.0f,  2.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,
		-2.0f,  2.0f,  0.0f, //
		 0.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,
	};
	static const GLfloat g_color_pyramid_buffer_data[] = {
		1.000f,  0.000f,  0.000f, //
		0.000f,  1.000f,  0.000f,
		0.700f,  0.000f,  0.700f,
		1.000f,  0.000f,  0.000f, //
		0.000f,  0.700f,  0.700f,
		0.700f,  0.000f,  0.700f,
		1.000f,  0.000f,  0.000f, //
		0.000f,  1.000f,  0.000f,
		0.100f,  0.100f,  0.100f,
		0.000f,  1.000f,  0.000f, //
		0.700f,  0.000f,  0.700f,
		0.100f,  0.100f,  0.100f,
		0.700f,  0.000f,  0.700f, //
		0.609f,  0.115f,  0.436f,
		0.000f,  0.000f,  0.000f,
		0.000f,  0.700f,  0.700f, //
		1.000f,  0.000f,  0.000f,
		0.100f,  0.100f,  0.100f,
	};
	// Fill vertex buffer
	GLuint vertexPyramidBuffer;
	glGenBuffers(1, &vertexPyramidBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPyramidBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_pyramid_buffer_data), g_vertex_pyramid_buffer_data, GL_STATIC_DRAW);
	// Fill color buffer
	GLuint colorPyramidBuffer;
	glGenBuffers(1, &colorPyramidBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorPyramidBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_pyramid_buffer_data), g_color_pyramid_buffer_data, GL_STATIC_DRAW);

	float time = 0;
	long long last_ms = 0;
	long long last_step = 0;
	do{
		// calculate fps
		++last_step;
		if (last_step % 10 == 0) {
			printf("FPS: %f\n", 10 * (1000.0f / (current_ms() - last_ms)));
			last_ms = current_ms();
		}

		// time for object movement
		time += 0.01;
		
		// Recalculate MVP
		// Angle of view: 45, ratio: 1:1, display distance: 0.1 <-> 100
		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
		// Camera: pos(x, y, z), look(0, 0, 0), head(0, 1, 0)
		float posx = 6 * std::sin(time);
		float posy = 4 * std::cos(time);
		float posz = 4 * (std::sin(time) + std::cos(time));
		glm::mat4 View = glm::lookAt(glm::vec3(posx, posy, posz), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		// Model
		float crysis_time = 1.2 - std::pow(std::sin(time), 16);
		glm::mat4 modelScaleMatrix = glm::scale(glm::vec3(crysis_time));
		glm::vec3 modelRotationAxis(0.0f, 0.0f, 1.0f);
		glm::mat4 modelRotationMatrix = glm::rotate(crysis_time * 64, modelRotationAxis);
		glm::mat4 modelTranslationMatrix = glm::translate(glm::vec3(0.0f, 0.0f, -1.0f));
		glm::mat4 ModelNoRotation = modelTranslationMatrix * glm::mat4(1.0f) * modelScaleMatrix;
		glm::mat4 ModelNoScale = modelTranslationMatrix * modelRotationMatrix * glm::mat4(1.0f);
		// ModelViewProjection
		glm::mat4 MVPNoObjScale = Projection * View * ModelNoScale;
		glm::mat4 MVPNoObjRotation = Projection * View * ModelNoRotation;

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable buffers with attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Draw pyramid
		// Set [coordinates] buffer ID: 0, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, vertexPyramidBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Set [colors] buffer ID: 1, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, colorPyramidBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Use shader for Pyramid
		glUseProgram(programPyramidID);
		glUniformMatrix4fv(MatrixPyramidID, 1, GL_FALSE, &MVPNoObjRotation[0][0]);
		// Draw
		glDrawArrays(GL_TRIANGLES, 0, 6 * 3);

		// Start of drawing transparent things
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);

		// Draw triangles
		// Set [coordinates] buffer ID: 0, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, vertexTrianglesBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Set [colors] buffer ID: 1, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, colorTrianglesBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Use shader for Triangle1
		glUseProgram(programTriangle1ID);
		glUniformMatrix4fv(MatrixTriangle1ID, 1, GL_FALSE, &MVPNoObjScale[0][0]);
		// Draw
		glDrawArrays(GL_TRIANGLES, 0, 1 * 3);
		// Use shader for Triangle2
		glUseProgram(programTriangle2ID);
		glUniformMatrix4fv(MatrixTriangle2ID, 1, GL_FALSE, &MVPNoObjScale[0][0]);
		// Draw
		glDrawArrays(GL_TRIANGLES, 1 * 3, 1 * 3);

		// End of drawing transparent things
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

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
	glDeleteBuffers(1, &vertexTrianglesBuffer);
	glDeleteBuffers(1, &colorTrianglesBuffer);
	glDeleteProgram(programTriangle1ID);
	glDeleteProgram(programTriangle2ID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

