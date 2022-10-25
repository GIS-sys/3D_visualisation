// HIGH
// add lighting (https://www.youtube.com/watch?v=ZbszezwNSZU)

// MEDIUM
// load save in the next session? add_object - bind_buffer possibly
// clear memory when loading

// LOW
// delete objects if they are -s_deleted once in a while; add is_shown to disable without deleting
// key pressed / key was pressed to myScene
// clear all memory in the end (destructor)
// sort transparent?


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <chrono>
#include <vector>
#include <list>
#include <fstream>
#include <algorithm>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>


const float PI = glm::pi<float>();
const GLfloat EPSYLON = 0.001f;

const std::string TYPE_SKY = "sky";
const std::string TYPE_FIREBALL_HIT = "fireball_hit";
const std::string TYPE_FIREBALL = "fireball";
const std::string TYPE_PYRAMID_ENEMY = "pyramid_enemy";
const long long ENEMY_SPAWN_DELTA = 500;
const long long ENEMY_SPAWN_RADIUS = 10;
const long long FIREBALL_COOLDOWN = 300;
const long long SCREEN_WIDTH = 1024;
const long long SCREEN_HEIGHT = 1024;
const GLfloat PYRAMID_ENEMY_SPEED = 0.00001f;
const GLfloat PYRAMID_ENEMY_ROTATION_RATE = 0.001f;
const GLfloat FIREBALL_SPEED = 0.05f;
const GLfloat FIREBALL_ROTATION_RATE = 0.005f;
const GLfloat HUMAN_SPEED = 0.005f;
const GLfloat HUMAN_SPRINT_SPEED = 0.05f;
const GLfloat MOUSE_SPEED = 0.002f;
const GLfloat PITCH_SPEED = 0.003f;

const int KEY_EXIT = GLFW_KEY_ESCAPE;
const int KEY_FORWARD = GLFW_KEY_W;
const int KEY_LEFT = GLFW_KEY_A;
const int KEY_RIGHT = GLFW_KEY_D;
const int KEY_BACK = GLFW_KEY_S;
const int KEY_PITCH_LEFT = GLFW_KEY_Q;
const int KEY_PITCH_RIGHT = GLFW_KEY_E;
const int KEY_RISE = GLFW_KEY_SPACE;
const int KEY_LOWER = GLFW_KEY_LEFT_CONTROL;
const int KEY_SPRINT = GLFW_KEY_LEFT_SHIFT;
const int KEY_FIREBALL = GLFW_KEY_L;
const int KEY_SAVE = GLFW_KEY_O;
const int KEY_LOAD = GLFW_KEY_P;


vec3 calc_right(vec3 forward, vec3 head) {
	return cross(forward, head);
}

vec3 calc_forward(vec3 head, vec3 right) {
	return cross(head, right);
}

vec3 calc_head(vec3 right, vec3 forward) {
	return cross(right, forward);
}

long long current_ms() {
	using namespace std::chrono;
	return static_cast<long long>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

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

GLfloat random(float a, float b) {
	return ((float)std::rand()) / ((float)RAND_MAX) * (b - a) + a;
}


template <typename T>
std::ofstream& operator<<(std::ofstream& ofs, const std::vector<T>& vec) {
	ofs << vec.size() << std::endl;
	for (const auto& x : vec) {
		ofs << x << std::endl;
	}
	return ofs;
}

template <typename T>
std::ifstream& operator>>(std::ifstream& ifs, std::vector<T>& vec) {
	size_t size = 0;
	ifs >> size;
	vec.clear();
	vec.resize(size);
	for (auto& x : vec) {
		ifs >> x;
	}
	return ifs;
}

std::ofstream& operator<<(std::ofstream& ofs, const vec3& vec) {
	ofs << vec[0] << " " << vec[1] << " " << vec[2] << std::endl;
	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, vec3& vec) {
	ifs >> vec[0] >> vec[1] >> vec[2];
	return ifs;
}

std::ofstream& operator<<(std::ofstream& ofs, const mat4& vec) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ofs << vec[i][j] << " ";
		}
	}
	ofs << std::endl;
	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, mat4& vec) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ifs >> vec[i][j];
		}
	}
	return ifs;
}


struct MyObjectRaw {
	std::vector<GLfloat> verts;
	bool is_transparent = false;

	std::vector<GLfloat> colors;
	std::vector<GLfloat> normals;
	GLuint program_id = 0;
	GLuint texture_id = 0;
	std::string sampler;
	bool is_texture_instead_of_color = false;

	GLfloat collider_radius = 0;

	MyObjectRaw(std::vector<GLfloat> verts_buf, bool is_transparent, GLfloat collider_radius = 0) :
			   		verts(verts_buf), is_transparent(is_transparent), collider_radius(collider_radius) {
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

class MyScene {
private:
	struct MyObject {
	private:
		GLfloat collider_radius = 0;
	public:
		GLuint program = 0;
		GLuint texture = 0;
		GLuint matrix = 0;
		GLuint vertex = 0;
		GLuint normal = 0;
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
		glm::vec3 speed{0.0f, 0.0f, 0.0f};
		std::string label;

		glm::mat4 calculate_scale_matrix() const {
			return glm::scale(scale);
		}

		glm::mat4 calculate_rotation_matrix() const {
			return glm::rotate(rotation_angle, rotation_axis);
		}

		glm::mat4 calculate_translation_matrix() const {
			return glm::translate(translation);
		}

		glm::mat4 calculate_model_matrix() const {
			return calculate_translation_matrix() * calculate_rotation_matrix() * calculate_scale_matrix();
		}

		GLfloat get_collider_radius() const {
			return collider_radius * glm::length(scale) / sqrtf(3.0);
		}

		friend std::ofstream& operator<<(std::ofstream& ofs, const MyObject& obj) {
			ofs << obj.collider_radius << std::endl;
			ofs << obj.program << std::endl;
			ofs << obj.texture << std::endl;
			ofs << obj.matrix << std::endl;
			ofs << obj.vertex << std::endl;
			ofs << obj.normal << std::endl;
			ofs << obj.color << std::endl;
			ofs << "[" << obj.sampler << std::endl;
			ofs << obj.triangles_amount << std::endl;
			ofs << obj.is_transparent << std::endl;
			ofs << obj.is_texture_instead_of_color << std::endl;
			ofs << obj.is_deleted << std::endl;
			ofs << obj.rotation_angle << std::endl;
			ofs << "[" << obj.label << std::endl;
			ofs << obj.scale << obj.rotation_axis << obj.translation << obj.speed;
			return ofs;
		}

		friend std::ifstream& operator>>(std::ifstream& ifs, MyObject& obj) {
			ifs >> obj.collider_radius;
			ifs >> obj.program;
			ifs >> obj.texture;
			ifs >> obj.matrix;
			ifs >> obj.vertex;
			ifs >> obj.normal;
			ifs >> obj.color;
			ifs >> obj.sampler; obj.sampler = obj.sampler.substr(1);
			ifs >> obj.triangles_amount;
			ifs >> obj.is_transparent;
			ifs >> obj.is_texture_instead_of_color;
			ifs >> obj.is_deleted;
			ifs >> obj.rotation_angle;
			ifs >> obj.label; obj.label = obj.label.substr(1);
			ifs >> obj.scale >> obj.rotation_axis >> obj.translation >> obj.speed;
			return ifs;
		}

		friend class MyScene;
	};

	std::string cwd;
	GLuint VertexArrayID;
	GLuint ColorArrayID;
	GLuint NormalArrayID;
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
		glm::mat4 model_rotation = obj.calculate_rotation_matrix();
		glm::mat4 model = obj.calculate_model_matrix();
		glm::mat4 MVP = projection * view * model;

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
		// Set [normals] buffer ID: 2, size: 3, normalised: false
		glBindBuffer(GL_ARRAY_BUFFER, obj.normal);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Use shader
		glUseProgram(obj.program);
		glUniformMatrix4fv(obj.matrix, 1, GL_FALSE, &MVP[0][0]);
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		std::vector<vec3> light_colors{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
		std::vector<vec3> light_positions{{0.0f, 0.0f, 10.0f}, {10.0f, 0.0f, 0.0f}};
		int lights_amount = light_colors.size();
		glUniformMatrix4fv(glGetUniformLocation(obj.program, "model_matrix"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(obj.program, "model_rotation_matrix"), 1, GL_FALSE, &model_rotation[0][0]);
		glUniform3fv(glGetUniformLocation(obj.program, "camera_pos"), 1, &camera_pos[0]);
		glUniform3fv(glGetUniformLocation(obj.program, "camera_look"), 1, &camera_look[0]);
		glUniform1i(glGetUniformLocation(obj.program, "lights_amount"), lights_amount);
		glUniform3fv(glGetUniformLocation(obj.program, "light_colors"), light_colors.size(), &light_colors[0][0]);
		glUniform3fv(glGetUniformLocation(obj.program, "light_positions"), light_positions.size(), &light_positions[0][0]);
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

		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Homework 2", NULL, NULL);
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

		// without it random object will be displayed, not closest
		glEnable(GL_DEPTH_TEST);
		// without it there is no transparency
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		// edge case for transparent
		glDepthFunc(GL_LEQUAL);

		// Ensure we can capture the escape key being pressed below
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		// Hide the mouse and enable unlimited mouvement
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
		// Set the mouse at the center of the screen
		glfwPollEvents();
		glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

		// create memory for vertices
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
		glGenVertexArrays(1, &ColorArrayID);
		glBindVertexArray(ColorArrayID);
		glGenVertexArrays(1, &NormalArrayID);
		glBindVertexArray(NormalArrayID);
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

	size_t amount_of_objects() const {
		return objects.size();
	}

	MyObject& operator[](size_t k) {
		return objects[k];
	}

	glm::vec3 get_forward() const {
		return normalize(camera_look - camera_pos);
	}

	glm::vec3 get_right() const {
		return calc_right(get_forward(), camera_head);
	}

	void rotate_camera(GLfloat mouse_x_delta, GLfloat mouse_y_delta, GLfloat pitch_delta) {
		vec3 cur_forward = get_forward();
		vec3 cur_head = camera_head;
		vec3 cur_right = get_right();
		// turn up
		cur_forward = cur_forward * cosf(mouse_y_delta) - cur_head * sinf(mouse_y_delta);
		cur_head = calc_head(cur_right, cur_forward);
		// turn right
		cur_forward = cur_forward * cosf(mouse_x_delta) + cur_right * sinf(mouse_x_delta);
		cur_right = calc_right(cur_forward, cur_head);
		// pitch
		cur_right = cur_right * cosf(pitch_delta) + cur_head * sinf(pitch_delta);
		cur_head = calc_head(cur_right, cur_forward);
		// set
		camera_head = normalize(cur_head);
		camera_look = (camera_pos + cur_forward);
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

	size_t add_object(MyObjectRaw obj, std::string label) {
		GLuint matrixID = glGetUniformLocation(shaders[obj.program_id], "MVP");

		GLuint vertexBufferID;
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, obj.verts.size() * sizeof(GLfloat), obj.verts.data(), GL_STATIC_DRAW);
		GLuint colorBufferID;
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, obj.colors.size() * sizeof(GLfloat), obj.colors.data(), GL_STATIC_DRAW);
		obj.normals.clear();
		for (size_t i = 0; i < obj.verts.size() / 9; ++i) {
			GLfloat x1 = obj.verts[9 * i + 0];
			GLfloat y1 = obj.verts[9 * i + 1];
			GLfloat z1 = obj.verts[9 * i + 2];
			GLfloat x2 = obj.verts[9 * i + 3];
			GLfloat y2 = obj.verts[9 * i + 4];
			GLfloat z2 = obj.verts[9 * i + 5];
			GLfloat x3 = obj.verts[9 * i + 6];
			GLfloat y3 = obj.verts[9 * i + 7];
			GLfloat z3 = obj.verts[9 * i + 8];
			vec3 v1{x1 - x2, y1 - y2, z1 - z2};
			vec3 v2{x3 - x2, y3 - y2, z3 - z2};
			vec3 norm = cross(v1, v2);
			for (int j = 0; j < 3; ++j) {
				obj.normals.push_back(norm[0]);
				obj.normals.push_back(norm[1]);
				obj.normals.push_back(norm[2]);
			}
		}
		GLuint normalBufferID;
		glGenBuffers(1, &normalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ARRAY_BUFFER, obj.normals.size() * sizeof(GLfloat), obj.normals.data(), GL_STATIC_DRAW);

		MyObject new_object;
		new_object.program = shaders[obj.program_id];
		new_object.texture = textures[obj.texture_id];
		new_object.matrix = matrixID;
		new_object.vertex = vertexBufferID;
		new_object.normal = normalBufferID;
		new_object.color = colorBufferID;
		new_object.is_transparent = obj.is_transparent;
		new_object.collider_radius = obj.collider_radius;
		new_object.is_texture_instead_of_color = obj.is_texture_instead_of_color;
		new_object.triangles_amount = obj.verts.size() / 3;
		new_object.label = label;

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
		glEnableVertexAttribArray(2);

		// Start of drawing solid things
		for (auto& obj : objects) {
			if (obj.is_transparent || obj.is_deleted) continue;
			update_single_object(obj);
		}
		// End of drawing solid things

		// Start of drawing transparent things
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		//std::vector<MyObject> sorted_objects = objects;
		//std::sort(sorted_objects.begin(), sorted_objects.end(), [](const MyObject& a, const MyObject& b){ return 1.0f; });
		for (auto& obj : objects) {
			if (!obj.is_transparent || obj.is_deleted) continue;
			update_single_object(obj);
		}
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		// End of drawing transparent things

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		// Disable buffers with attributes

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void save(const std::string& filename) {
		std::ofstream file_to_save(filename, std::ofstream::out);
		file_to_save << "[" << cwd << std::endl;
		file_to_save << VertexArrayID << std::endl;
		file_to_save << projection_angle << std::endl;
		file_to_save << projection_ratio << std::endl;
		file_to_save << projection_dist_min << std::endl;
		file_to_save << projection_dist_max << std::endl;
		file_to_save << shaders << textures;
		file_to_save << objects;
		file_to_save << camera_pos << camera_look << camera_head << projection << view;
		file_to_save.close();
		printf("SAVE IS SUCCESFULL\n");
	}

	void load(const std::string& filename) {
		std::ifstream file_to_load(filename, std::ifstream::in);
		file_to_load >> cwd; cwd = cwd.substr(1);
		file_to_load >> VertexArrayID;
		file_to_load >> projection_angle;
		file_to_load >> projection_ratio;
		file_to_load >> projection_dist_min;
		file_to_load >> projection_dist_max;
		file_to_load >> shaders >> textures;
		file_to_load >> objects;
		file_to_load >> camera_pos >> camera_look >> camera_head >> projection >> view;
		file_to_load.close();
		printf("LOAD IS SUCCESFULL\n");
		save(filename + "loaded");
	}
};


std::vector<GLfloat> get_vert_sphere(GLfloat radius, int triangles_in_height, int triangles_in_width) {
	GLfloat angle_height = PI / triangles_in_height;
	GLfloat angle_width = 2 * PI / triangles_in_width;
	std::vector<GLfloat> result;
	std::vector<vec3> lst_points(triangles_in_width + 1, {0, 0, radius});
	for (int k = 1; k <= triangles_in_height; ++k) {
		// calculate new points
		std::vector<vec3> new_points;
		if (k == triangles_in_height) {
			new_points = std::vector<vec3>(triangles_in_width + 1, {0, 0, -radius});
		} else {
			for (int j = 0; j < triangles_in_width; ++j) {
				GLfloat cur_angle_height = angle_height * k;
				GLfloat cur_angle_width = angle_width * j;
				new_points.push_back({radius*sinf(cur_angle_height)*cosf(cur_angle_width),
									radius*sinf(cur_angle_height)*sinf(cur_angle_width),
									radius*cosf(cur_angle_height)});
			}
			new_points.push_back(new_points[0]);
		}
		// add triangles
		for (int j = 0; j < triangles_in_width; ++j) {
			if (new_points[j+0] != new_points[j+1]) {
				result.push_back(new_points[j+1][0]); result.push_back(new_points[j+1][1]); result.push_back(new_points[j+1][2]);
				result.push_back(new_points[j+0][0]); result.push_back(new_points[j+0][1]); result.push_back(new_points[j+0][2]);
				result.push_back(lst_points[j+0][0]); result.push_back(lst_points[j+0][1]); result.push_back(lst_points[j+0][2]);
			}
			if (lst_points[j+0] != lst_points[j+1]) {
				result.push_back(new_points[j+1][0]); result.push_back(new_points[j+1][1]); result.push_back(new_points[j+1][2]);
				result.push_back(lst_points[j+0][0]); result.push_back(lst_points[j+0][1]); result.push_back(lst_points[j+0][2]);
				result.push_back(lst_points[j+1][0]); result.push_back(lst_points[j+1][1]); result.push_back(lst_points[j+1][2]);
			}
		}
		// next layer
		lst_points = new_points;
	}
	return result;
}

std::vector<GLfloat> get_uv_simple(const std::vector<GLfloat>& vert) {
	std::vector<GLfloat> result;
	for (int k = 0; k < vert.size() / 9; ++k) {
		result.push_back(0); result.push_back(0);
		result.push_back(0); result.push_back(1);
		result.push_back(1); result.push_back(0);
	}
	return result;
}

std::vector<GLfloat> get_color_simple(const std::vector<GLfloat>& vert, vec3 color) {
	std::vector<GLfloat> result;
	for (int k = 0; k < vert.size() / 3; ++k) {
		result.push_back(color[0]); result.push_back(color[1]); result.push_back(color[2]);
	}
	return result;
}

std::vector<GLfloat> get_uv_sphere(const std::vector<GLfloat>& vert) {
	GLfloat radius = sqrtf(vert[0] * vert[0] + vert[1] * vert[1] + vert[2] * vert[2]);
	std::vector<GLfloat> result;
	// map cylider on sphere
	for (int k = 0; k < vert.size() / 3; ++k) {
		GLfloat angle_width = atan2f(vert[3 * k + 1], vert[3 * k + 0]) + PI;
		GLfloat angle_height = PI / 2 - acosf(vert[3 * k + 2] / radius);
		GLfloat uvx = angle_width / (2 * PI);
		GLfloat uvy = (sinf(angle_height) + 1.0f) / 2.0f;
		uvx = std::min(1.0f - EPSYLON, std::max(EPSYLON, uvx));
		uvy = std::min(1.0f - EPSYLON, std::max(EPSYLON, uvy));
		result.push_back(uvx); result.push_back(uvy);
	}
	// edge case: UV gap
	const GLfloat big_gap = 0.9f;
	for (int k = 0; k < result.size() / 6; ++k) {
		GLfloat x1 = result[6 * k + 0]; 
		GLfloat x2 = result[6 * k + 2];
		GLfloat x3 = result[6 * k + 4];
		//continue;
		if (abs(x1 - x2) > big_gap || abs(x1 - x3) > big_gap || abs(x2 - x3) > big_gap) {
			bool side1 = (x1 > 0.5);
			bool side2 = (x2 > 0.5);
			bool side3 = (x3 > 0.5);
			bool true_side = 1 ^ side1 ^ side2 ^ side3;
			if (true_side) {
				if (!side1) result[6 * k + 0] = 1 - EPSYLON;
				if (!side2) result[6 * k + 2] = 1 - EPSYLON;
				if (!side3) result[6 * k + 4] = 1 - EPSYLON;
			} else {
				if (side1) result[6 * k + 0] = EPSYLON;
				if (side2) result[6 * k + 2] = EPSYLON;
				if (side3) result[6 * k + 4] = EPSYLON;
			}
		}
	}
	return result;
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
	myScene.projection_ratio = SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT;
	myScene.projection_dist_min = 0.1f;
	myScene.projection_dist_max = 128.0f;
	// Camera: pos(0, 0, 0), look(0, 1, 0), head(0, 1, 0)
	myScene.camera_pos = glm::vec3(0, 0, 0);
	myScene.camera_look = glm::vec3(1, 0, 0);
	myScene.camera_head = glm::vec3(0, 0, -1);


	// load shaders
	size_t shader_fireball = myScene.load_shader("shaders/Fireball");
	size_t shader_pyramid_enemy = myScene.load_shader("shaders/PyramidEnemy");
	size_t shader_fireball_hit = myScene.load_shader("shaders/FireballHit");
	size_t shader_sky = myScene.load_shader("shaders/Sky");

	// load textures
	size_t texture_uv_fireball = myScene.load_texture("textures/fireball.bmp");
	size_t texture_uv_sky = myScene.load_texture("textures/sky.bmp");


	// sky
	std::vector<GLfloat> sky_vert = get_vert_sphere(99.0f, 32, 64);
	std::vector<GLfloat> sky_uv = get_uv_sphere(sky_vert);
	MyObjectRaw sky_raw(sky_vert, false, 99.0f);
	sky_raw.set_texture(texture_uv_sky, "textureSamplerSky", sky_uv, shader_sky);

	// fireball
	std::vector<GLfloat> fireball_vert = get_vert_sphere(0.5, 16, 32);
	std::vector<GLfloat> fireball_uv = get_uv_sphere(fireball_vert);
	MyObjectRaw fireball_raw(fireball_vert, false, 0.5);
	fireball_raw.set_texture(texture_uv_fireball, "textureSamplerFireball", fireball_uv, shader_fireball);

	// fireball hit sphere
	std::vector<GLfloat> fireball_hit_vert = get_vert_sphere(0.25, 4, 8);
	std::vector<GLfloat> fireball_hit_col = get_color_simple(fireball_hit_vert, {0, 1, 0});
	MyObjectRaw fireball_hit_raw(fireball_hit_vert, true, 0.25);
	fireball_hit_raw.set_colors(fireball_hit_col, shader_fireball_hit);

	// enemy
	/*std::vector<GLfloat> pyramid_enemy_vert = vector_from_file(cwd + "models/pyramid.vert");
	std::vector<GLfloat> pyramid_enemy_col = vector_from_file(cwd + "models/pyramid.col");
	MyObjectRaw pyramid_enemy_raw(pyramid_enemy_vert, false, sqrtf(3) / 2);
	pyramid_enemy_raw.set_colors(pyramid_enemy_col, shader_pyramid_enemy);*/
	std::vector<GLfloat> pyramid_enemy_vert = get_vert_sphere(sqrtf(3), 128, 256);
	std::vector<GLfloat> pyramid_enemy_col = get_color_simple(pyramid_enemy_vert, {1, 1, 1});
	MyObjectRaw pyramid_enemy_raw(pyramid_enemy_vert, false, sqrtf(3) / 2);
	pyramid_enemy_raw.set_colors(pyramid_enemy_col, shader_pyramid_enemy);


	myScene.add_object(sky_raw, TYPE_SKY);
	myScene.add_object(fireball_hit_raw, TYPE_FIREBALL_HIT);
	const int MAX_EFFECT_FIREBALL_HIT = 30;
	int effect_fireball_hit = -1;

	////////////
	size_t tmp = myScene.add_object(fireball_raw, "");
	myScene[tmp].translation = {10.0, 0.0, 0.0};
	tmp = myScene.add_object(fireball_raw, "");
	myScene[tmp].translation = {0.0, 0.0, 10.0};
	////////////

	long long ticks = 0;
	long long last_fps_ms = 0;
	long long enemy_last_spawn_time = 0;
	long long fireball_last_spawn_time = 0;
	long long last_frame_time = 0;
	bool save_button_was_pressed = false;
	bool load_button_was_pressed = false;
	do {
		float delta_time = current_ms() - last_frame_time;
		last_frame_time = current_ms();
		// calculate fps
		++ticks;
		if (ticks % 10 == 0) {
			printf("FPS: %f\n", 10 * (1000.0f / (current_ms() - last_fps_ms)));
			last_fps_ms = current_ms();
		}


		// get mouse pos
		double mouse_x_delta = 0;
		double mouse_y_delta = 0;
		glfwGetCursorPos(window, &mouse_x_delta, &mouse_y_delta);
		glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		mouse_x_delta -= SCREEN_WIDTH / 2;
		mouse_y_delta -= SCREEN_HEIGHT / 2;
		mouse_x_delta *= MOUSE_SPEED;
		mouse_y_delta *= MOUSE_SPEED;


		// get all objects from scene
		std::list<size_t> pyramid_enemies;
		std::list<size_t> sphere_fireballs;
		size_t sky = 0;
		size_t fireball_hit = 0;
		for (size_t obj = 0; obj < myScene.amount_of_objects(); ++obj) {
			if (myScene[obj].label == TYPE_SKY) {
				sky = obj;
			} else if (myScene[obj].label == TYPE_FIREBALL_HIT) {
				fireball_hit = obj;
			} else if (myScene[obj].label == TYPE_PYRAMID_ENEMY) {
				if (!myScene[obj].is_deleted) pyramid_enemies.push_back(obj);
			} else if (myScene[obj].label == TYPE_FIREBALL) {
				if (!myScene[obj].is_deleted) sphere_fireballs.push_back(obj);
			}
		}
		//myScene[sky].is_deleted = true;


		// delete objects
		bool boom = false;
		auto fireball = sphere_fireballs.begin();
		while (fireball != sphere_fireballs.end()) {
			auto enemy = pyramid_enemies.begin();
			while (enemy != pyramid_enemies.end()) {
				GLfloat dist = distance(myScene[*enemy].translation, myScene[*fireball].translation);
				GLfloat coll = myScene[*enemy].get_collider_radius() + myScene[*fireball].get_collider_radius();
				if (dist < coll) {
					boom = true;
					myScene[*fireball].is_deleted = true;
					myScene[*enemy].is_deleted = true;
					sphere_fireballs.erase(fireball++);
					pyramid_enemies.erase(enemy++);
					break;
				}
				++enemy;
			}
			if (boom) break;
			++fireball;
		}


		// create objects
		if (current_ms() - enemy_last_spawn_time >= ENEMY_SPAWN_DELTA) {
			size_t pyramid = myScene.add_object(pyramid_enemy_raw, TYPE_PYRAMID_ENEMY);
			myScene[pyramid].translation = glm::vec3(random(-1, 1) * ENEMY_SPAWN_RADIUS,
												     random(-1, 1) * ENEMY_SPAWN_RADIUS,
													 random(-1, 1) * ENEMY_SPAWN_RADIUS);
			//myScene[pyramid].rotation_angle = random(-1, 1) * PI;
			myScene[pyramid].speed = glm::vec3(random(-1, 1), random(-1, 1), 0) * PYRAMID_ENEMY_SPEED;
			pyramid_enemies.push_back(pyramid);
			enemy_last_spawn_time = current_ms();
		}
		if (glfwGetKey(window, KEY_FIREBALL) == GLFW_PRESS) {
			if (current_ms() - fireball_last_spawn_time >= FIREBALL_COOLDOWN) {
			  	size_t fireball = myScene.add_object(fireball_raw, TYPE_FIREBALL);
			  	myScene[fireball].translation = myScene.camera_pos + myScene.get_forward() * (float)myScene[fireball].get_collider_radius() * 2.0f;
			  	myScene[fireball].translation += (-myScene.camera_head + myScene.get_right()) * 0.5f;
				myScene[fireball].rotation_axis = myScene.get_forward();
			  	myScene[fireball].speed = myScene.get_forward() * FIREBALL_SPEED;
			  	sphere_fireballs.push_back(fireball);
			  	fireball_last_spawn_time = current_ms();
			}
		}


		// change camera pos
		vec3 delta_cam_pos;
		GLfloat current_speed = ((glfwGetKey(window, KEY_SPRINT ) == GLFW_PRESS) ? HUMAN_SPRINT_SPEED : HUMAN_SPEED);
		if (glfwGetKey(window, KEY_FORWARD) == GLFW_PRESS) {
			delta_cam_pos += myScene.get_forward();
		}
		if (glfwGetKey(window, KEY_BACK) == GLFW_PRESS) {
			delta_cam_pos += -myScene.get_forward();
		}
		if (glfwGetKey(window, KEY_LEFT) == GLFW_PRESS) {
			delta_cam_pos += -myScene.get_right();
		}
		if (glfwGetKey(window, KEY_RIGHT) == GLFW_PRESS) {
			delta_cam_pos += myScene.get_right();
		}
		if (glfwGetKey(window, KEY_RISE) == GLFW_PRESS) {
			delta_cam_pos += myScene.camera_head;
		}
		if (glfwGetKey(window, KEY_LOWER) == GLFW_PRESS) {
			delta_cam_pos += -myScene.camera_head;
		}
		if (glm::length(delta_cam_pos) > 0) {
			delta_cam_pos = normalize(delta_cam_pos) * current_speed * delta_time;
			myScene.camera_pos += delta_cam_pos;
			myScene.camera_look += delta_cam_pos;
		}
		// change camera turning
		GLfloat pitch_delta = 0.0f;
		if (glfwGetKey(window, KEY_PITCH_LEFT) == GLFW_PRESS) {
			pitch_delta += PITCH_SPEED * delta_time;
		}
		if (glfwGetKey(window, KEY_PITCH_RIGHT) == GLFW_PRESS) {
			pitch_delta -= PITCH_SPEED * delta_time;
		}
		myScene.rotate_camera(mouse_x_delta, mouse_y_delta, pitch_delta);


		// Change models
		for (size_t fireball : sphere_fireballs) {
			myScene[fireball].rotation_angle += FIREBALL_ROTATION_RATE * delta_time;
			myScene[fireball].translation += myScene[fireball].speed * delta_time;
		}
		for (size_t enemy : pyramid_enemies) {
			myScene[enemy].rotation_angle += PYRAMID_ENEMY_ROTATION_RATE * delta_time;
			myScene[enemy].translation += myScene[enemy].speed * delta_time;
		}
		myScene[sky].translation = myScene.camera_pos;


		// Effects
		if (boom) {
			effect_fireball_hit = MAX_EFFECT_FIREBALL_HIT;
		}
		// Effect of fireball
		if (effect_fireball_hit >= 0) {
			--effect_fireball_hit;
			myScene[fireball_hit].is_deleted = false;
			myScene[fireball_hit].translation = myScene.camera_pos;
		} else {
			myScene[fireball_hit].is_deleted = true;
		}


		myScene.update();


		bool new_load_button_was_pressed = (glfwGetKey(window, KEY_LOAD) == GLFW_PRESS);
		bool new_save_button_was_pressed = (glfwGetKey(window, KEY_SAVE) == GLFW_PRESS);
		// load scene
		if (load_button_was_pressed && !new_load_button_was_pressed) myScene.load("/home/gordei/tmp/file");
		// save scene
		if (save_button_was_pressed && !new_save_button_was_pressed) myScene.save("/home/gordei/tmp/file");
		// update buttons states
		load_button_was_pressed = new_load_button_was_pressed;
		save_button_was_pressed = new_save_button_was_pressed;
	} while(glfwGetKey(window, KEY_EXIT) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return 0;
}
