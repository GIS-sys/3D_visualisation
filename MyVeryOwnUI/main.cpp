// manage textures ////////////////////////////////////////////////////
// load textures and shaders separately, connect to them via index

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <chrono>
#include <vector>
#include <fstream>

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

struct MyObjectRaw {
	std::vector<GLfloat> verts;
	bool is_transparent = false;

	std::vector<GLfloat> colors;
	GLuint program_id = 0;
	GLuint texture_id = 0;
	std::string sampler;
	bool is_texture_instead_of_color = false;

	MyObjectRaw(std::vector<GLfloat> verts_buf, bool is_transparent = true) : verts(verts_buf), is_transparent(is_transparent) {
	}

	void set_colors(std::vector<GLfloat> colors_buf, size_t new_program_id) {
		is_texture_instead_of_color = false;
		colors = colors_buf;
		program_id = new_program_id;
	}

	void set_texture(GLuint new_texture_id, std::string new_sampler, std::vector<GLfloat> colors_buf, size_t new_program_id) {
		is_texture_instead_of_color = true;
		texture_id = new_texture_id;
		colors = colors_buf;
		program_id = new_program_id;
		sampler = new_sampler;
	}
};

struct MyObject {
	GLuint program = 0;
	GLuint texture = 0;
	GLuint matrix = 0;
	GLuint vertex = 0;
	GLuint color = 0;
	std::string sampler;
	int triangles_amount = 0;

	bool is_transparent = false;
	bool is_texture_instead_of_color = false;
	bool is_deleted = false;

	glm::vec3 scale{1.0f, 1.0f, 1.0f};
	glm::vec3 rotation_axis{0.0f, 0.0f, 1.0f};
	GLfloat rotation_angle = 0.0f;
	glm::vec3 translation{0.0f, 0.0f, 0.0f};

	glm::mat4 calculate_model() {
		glm::mat4 modelScaleMatrix = glm::scale(scale);
		glm::mat4 modelRotationMatrix = glm::rotate(rotation_angle, rotation_axis);
		glm::mat4 modelTranslationMatrix = glm::translate(translation);
		return modelTranslationMatrix * modelRotationMatrix * modelScaleMatrix;
	}
};

class MyScene {
private:
	std::string cwd;
	GLuint VertexArrayID;
	std::vector<MyObject> objects;
	std::vector<GLuint> shaders;
	std::vector<GLuint> textures;

	glm::mat4 projection;
	glm::mat4 view;

	void recalculate_projection() {
		projection = glm::perspective(projection_angle, projection_ratio, projection_dist_min, projection_dist_max);
	}

	void recalculate_view() {
		view = glm::lookAt(camera_pos, camera_look, camera_head);
	}

	void update_single_object(MyObject& obj) {
		glm::mat4 MVP = projection * view * obj.calculate_model();

		if (obj.is_texture_instead_of_color) {
			// Get a handle for our "...Sampler" uniform
			GLuint TextureID  = glGetUniformLocation(obj.program, obj.sampler.c_str());
			// Bind our texture in Texture Unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, obj.texture);
			// Set our "myTextureSampler" sampler to use Texture Unit 0
			glUniform1i(TextureID, 0);
		}

		// Set [coordinates] buffer ID: 0, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, obj.vertex);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Set [colors] buffer ID: 1, size: 3, normalised: false
		int color_size = 3;
		if (obj.is_texture_instead_of_color) color_size = 2;
		glBindBuffer(GL_ARRAY_BUFFER, obj.color);
		glVertexAttribPointer(1, color_size, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Use shader
		glUseProgram(obj.program);
		glUniformMatrix4fv(obj.matrix, 1, GL_FALSE, &MVP[0][0]);
		// Draw
		glDrawArrays(GL_TRIANGLES, 0, obj.triangles_amount);
	}

public:
	GLfloat projection_angle;
	GLfloat projection_ratio;
	GLfloat projection_dist_min;
	GLfloat projection_dist_max;

	glm::vec3 camera_pos;
	glm::vec3 camera_look;
	glm::vec3 camera_head;

	MyScene(const std::string& cwd) : cwd(cwd) {
		// Initialise window
		if( !glfwInit() )
		{
			fprintf( stderr, "Failed to initialize GLFW\n" );
			exit(1);
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow( 1024, 1024, "Homework 2", NULL, NULL);
		if( window == NULL ){
			fprintf( stderr, "Failed to open GLFW window.\n" );
			glfwTerminate();
			exit(1);
		}
		glfwMakeContextCurrent(window);

		// Initialize extensions
		glewExperimental = true; // For some reason use max version
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			exit(1);
		}

		// without it random object will be displyed, not closest
		glEnable(GL_DEPTH_TEST);
		// without it there is no transparency
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		// edge case for transparent
		glDepthFunc(GL_LEQUAL);

		// create memory for vertices
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
	}

	~MyScene() {
		// Cleanup
		for (auto& obj : objects) {
			glDeleteBuffers(1, &obj.vertex);
			glDeleteBuffers(1, &obj.color);
			glDeleteProgram(obj.program);
		}
		glDeleteVertexArrays(1, &VertexArrayID);
		// Close OpenGL window and terminate GLFW
		glfwTerminate();
	}

	MyObject& operator[](size_t k) {
		return objects[k];
	}

	[[nodiscard]] size_t load_shader(const std::string& shader_path) {
		GLuint programID = LoadShaders((cwd + shader_path + ".vertexshader").c_str(),
								   	   (cwd + shader_path+ ".fragmentshader").c_str());
		shaders.push_back(programID);
		return shaders.size() - 1;
	}

	[[nodiscard]] size_t load_texture(const std::string& texture_path) {
		GLuint textureID = loadBMP_custom((cwd + texture_path).c_str());
		textures.push_back(textureID);
		return textures.size() - 1;
	}

	[[nodiscard]] size_t add_object(MyObjectRaw obj) {
		GLuint matrixID = glGetUniformLocation(shaders[obj.program_id], "MVP");

		GLuint vertexBufferID;
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, obj.verts.size() * sizeof(GLfloat), obj.verts.data(), GL_STATIC_DRAW);
		GLuint colorBufferID;
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, obj.colors.size() * sizeof(GLfloat), obj.colors.data(), GL_STATIC_DRAW);

		MyObject new_object;
		new_object.program = shaders[obj.program_id];
		new_object.texture = textures[obj.texture_id];
		new_object.matrix = matrixID;
		new_object.vertex = vertexBufferID;
		new_object.color = colorBufferID;
		new_object.is_transparent = obj.is_transparent;
		new_object.is_texture_instead_of_color = obj.is_texture_instead_of_color;
		new_object.triangles_amount = obj.verts.size() / 3;
		objects.push_back(new_object);
		return objects.size() - 1;
	}

	void update() {
		recalculate_view();
		recalculate_projection();

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable buffers with attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Start of drawing solid things
		for (auto& obj : objects) {
			if (obj.is_transparent || obj.is_deleted) continue;
			update_single_object(obj);
		}
		// End of drawing solid things

		// Start of drawing transparent things
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		for (auto& obj : objects) {
			if (!obj.is_transparent || obj.is_deleted) continue;
			update_single_object(obj);
		}
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		// End of drawing transparent things

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		// Disable buffers with attributes

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
};

std::vector<GLfloat> vector_from_file(std::string filepath) {
	std::vector<GLfloat> ans;
	std::ifstream ifs;
	ifs.open(filepath);
	GLfloat number;
	while (ifs >> number) {
		ans.push_back(number);
	}
	ifs.close();
	return ans;
}

int main(int argc, char* argv[])
{
	// remember current working directory
	char dir[1024];
   	getcwd( dir, sizeof( dir ) );
   	std::string cwd = dir;
	cwd += "/../homework02/";


	MyScene myScene(cwd);

	// configure initial scene position
	// View: Angle of view: 45, ratio: 1:1, display distance: 0.1 <-> 100
	myScene.projection_angle = glm::radians(45.0f);
	myScene.projection_ratio = 1.0f / 1.0f;
	myScene.projection_dist_min = 0.1f;
	myScene.projection_dist_max = 100.0f;
	// Camera: pos(0, 0, 0), look(0, 0, 0), head(0, 1, 0)
	myScene.camera_pos = glm::vec3(0, 0, 0);
	myScene.camera_look = glm::vec3(0, 0, 0);
	myScene.camera_head = glm::vec3(0, 1, 0);


	// load shaders
	size_t shaderTriangle1 = myScene.load_shader("shaders/Triangle1");
	size_t shaderTriangle2 = myScene.load_shader("shaders/Triangle2");
	size_t shaderPyramid = myScene.load_shader("shaders/Pyramid");

	// load textures
	size_t textureUVTemplate = myScene.load_texture("textures/uvtemplate.bmp");


	// triangle 1
	std::vector<GLfloat> triangle1_vert = vector_from_file(cwd + "models/triangle1.vert");
	std::vector<GLfloat> triangle1_uv = vector_from_file(cwd + "models/triangle1.uv");
	MyObjectRaw triangle1_raw(triangle1_vert, true);
	triangle1_raw.set_texture(textureUVTemplate, "textureSamplerTriangle1", triangle1_uv, shaderTriangle1);

	// triangle 2
	std::vector<GLfloat> triangle2_vert = vector_from_file(cwd + "models/triangle2.vert");
	std::vector<GLfloat> triangle2_col = vector_from_file(cwd + "models/triangle2.col");
	MyObjectRaw triangle2_raw(triangle2_vert, true);
	triangle2_raw.set_colors(triangle2_col, shaderTriangle2);

	// pyramid
	std::vector<GLfloat> pyramid_vert = vector_from_file(cwd + "models/pyramid.vert");
	std::vector<GLfloat> pyramid_uv = vector_from_file(cwd + "models/pyramid.uv");
	MyObjectRaw pyramid_raw(pyramid_vert, false);
	pyramid_raw.set_texture(textureUVTemplate, "textureSamplerPyramid", pyramid_uv, shaderPyramid);


	// push object to the scene
	size_t triangle1 = myScene.add_object(triangle1_raw);
	myScene[triangle1].rotation_axis = glm::vec3(0.0f, 0.0f, 1.0f);
	myScene[triangle1].translation = glm::vec3(0.0f, 0.0f, -1.0f);

	size_t triangle2 = myScene.add_object(triangle2_raw);
	myScene[triangle2].rotation_axis = glm::vec3(0.0f, 0.0f, 1.0f);
	myScene[triangle2].translation = glm::vec3(0.0f, 0.0f, -1.0f);

	size_t pyramid = myScene.add_object(pyramid_raw);
	myScene[pyramid].translation = glm::vec3(0.0f, 0.0f, -1.0f);


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

		// change camera
		float posx = 6 * std::sin(time);
		float posy = 4 * std::cos(time);
		float posz = 4 * (std::sin(time) + std::cos(time));
		myScene.camera_pos = glm::vec3(posx, posy, posz);


		// Change models
		if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS) {
			myScene[triangle1].translation[2] += 0.01;
		}
		if (glfwGetKey(window, GLFW_KEY_B ) == GLFW_PRESS) {
			myScene[triangle1].translation[2] -= 0.01;
		}

		if (last_step % 200 == 0) {
			size_t new_pyramid = myScene.add_object(pyramid_raw);
			myScene[pyramid].is_deleted = true;
			myScene[new_pyramid].translation[0] = -last_step / 200;
		}

		float crysis_time = 1.2 - std::pow(std::sin(time), 16);
		myScene[triangle1].rotation_angle = crysis_time * 64;
		myScene[triangle2].rotation_angle = crysis_time * 64;
		myScene[pyramid].scale = glm::vec3(crysis_time);

		myScene.update();
	}
	while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return 0;
}
