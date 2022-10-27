// LOW
// MVP for ui
// bad memory clearment - clear_ui deletes
// key pressed / key was pressed to myScene
// delete objects if they are is_deleted once in a while; add is_shown to disable without deleting
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
//using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>


const float PI = glm::pi<float>();
const float E = glm::pi<float>();
const GLfloat EPSYLON = 0.001f;

const std::string TYPE_SKY = "sky";
const std::string TYPE_FIREBALL_HIT = "fireball_hit";
const std::string TYPE_FIREBALL = "fireball";
const std::string TYPE_PYRAMID_ENEMY = "pyramid_enemy";

const std::string MOON_LIGHT_LABEL = "moon";
const glm::vec4 MOON_LIGHT_COLOR = {0.1, 0.1, 0.2, 1.0};
const glm::vec3 MOON_LIGHT_DIRECTION = {0.0, 3.0, -1.0};
const glm::vec3 SKY_LIGHT_COLOR = {0.05, 0.05, 0.01};
const glm::vec3 FIREBALL_LIGHT_COLOR = {1.0, 0.7, 0.0};
const float FIREBALL_LIGHT_INTENSITY = 1.0;
const long long ENEMY_SPAWN_DELTA = 500;
const long long ENEMY_SPAWN_RADIUS = 20;
const long long FIREBALL_COOLDOWN = 200;
const int MAX_EFFECT_FIREBALL_HIT = 10;
const long long SCREEN_WIDTH = 1024;
const long long SCREEN_HEIGHT = 1024;
const GLfloat PYRAMID_ENEMY_SPEED = 0.01f;
const GLfloat PYRAMID_ENEMY_ROTATION_RATE = 0.001f;
const GLfloat FIREBALL_SPEED = 0.05f;
const GLfloat FIREBALL_ROTATION_RATE = 0.005f;
const GLfloat HUMAN_SPEED = 0.01f;
const GLfloat HUMAN_SPRINT_SPEED = 0.05f;
const GLfloat MOUSE_SPEED = 0.002f;
const GLfloat PITCH_SPEED = 0.003f;
const GLfloat DESPAWN_DISTANCE_IN_MAX_DISTANCE = 2.0f;
const std::string FILENAME_SAVE = "/home/gordei/tmp/file";
const GLfloat DIGIT_SCALE_RATE = 0.7f;

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
const int KEY_STOP_OBJECT_MOVEMENT = GLFW_KEY_M;
const int KEY_RESUME_OBJECT_MOVEMENT = GLFW_KEY_N;

const int MAX_LIGHTS = 256;


glm::vec3 calc_right(glm::vec3 forward, glm::vec3 head) {
	return cross(forward, head);
}

glm::vec3 calc_forward(glm::vec3 head, glm::vec3 right) {
	return cross(head, right);
}

glm::vec3 calc_head(glm::vec3 right, glm::vec3 forward) {
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



std::ofstream& operator<<(std::ofstream& ofs, const glm::vec3& vec) {
	ofs << vec[0] << " " << vec[1] << " " << vec[2] << std::endl;
	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, glm::vec3& vec) {
	ifs >> vec[0] >> vec[1] >> vec[2];
	return ifs;
}

std::ofstream& operator<<(std::ofstream& ofs, const glm::vec4& vec) {
	ofs << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3] << std::endl;
	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, glm::vec4& vec) {
	ifs >> vec[0] >> vec[1] >> vec[2] >> vec[3];
	return ifs;
}

std::ofstream& operator<<(std::ofstream& ofs, const glm::mat4& vec) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ofs << vec[i][j] << " ";
		}
	}
	ofs << std::endl;
	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, glm::mat4& vec) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ifs >> vec[i][j];
		}
	}
	return ifs;
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

template <>
std::ofstream& operator<<(std::ofstream& ofs, const std::vector<std::string>& vec) {
	ofs << vec.size() << std::endl;
	for (const auto& x : vec) {
		ofs << "[" << x << std::endl;
	}
	return ofs;
}

template <>
std::ifstream& operator>>(std::ifstream& ifs, std::vector<std::string>& vec) {
	size_t size = 0;
	ifs >> size;
	vec.clear();
	vec.resize(size);
	for (auto& x : vec) {
		ifs >> x;
		x = x.substr(1);
	}
	return ifs;
}


// after creating need to use either of set_ functions
struct MyObjectRaw {
private:
	void bind_arrays() {
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
		glGenBuffers(1, &normalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
	}

public:
	std::vector<GLfloat> verts;
	bool is_transparent = false;
	std::vector<GLfloat> colors;
	std::vector<GLfloat> normals;
	GLuint vertexBufferID;
	GLuint colorBufferID;
	GLuint normalBufferID;
	GLuint program_id = 0;
	GLuint texture_id = 0;
	std::string sampler;
	bool is_texture_instead_of_color = false;

	GLfloat collider_radius = 0;

	MyObjectRaw(std::vector<GLfloat> verts_buf, bool is_transparent, GLfloat collider_radius = 0) :
			   		verts(verts_buf), is_transparent(is_transparent), collider_radius(collider_radius) {
		normals.clear();
		for (size_t i = 0; i < verts.size() / 9; ++i) {
			GLfloat x1 = verts[9 * i + 0];
			GLfloat y1 = verts[9 * i + 1];
			GLfloat z1 = verts[9 * i + 2];
			GLfloat x2 = verts[9 * i + 3];
			GLfloat y2 = verts[9 * i + 4];
			GLfloat z2 = verts[9 * i + 5];
			GLfloat x3 = verts[9 * i + 6];
			GLfloat y3 = verts[9 * i + 7];
			GLfloat z3 = verts[9 * i + 8];
			glm::vec3 v1{x1 - x2, y1 - y2, z1 - z2};
			glm::vec3 v2{x3 - x2, y3 - y2, z3 - z2};
			glm::vec3 norm = cross(v1, v2);
			for (int j = 0; j < 3; ++j) {
				normals.push_back(norm[0]);
				normals.push_back(norm[1]);
				normals.push_back(norm[2]);
			}
		}
	}

	void set_colors(std::vector<GLfloat> colors_buf, size_t new_program_id) {
		is_texture_instead_of_color = false;
		colors = colors_buf;
		program_id = new_program_id;
		bind_arrays();
	}

	void set_texture(GLuint new_texture_id, std::string new_sampler, std::vector<GLfloat> colors_buf, size_t new_program_id) {
		is_texture_instead_of_color = true;
		texture_id = new_texture_id;
		colors = colors_buf;
		program_id = new_program_id;
		sampler = new_sampler;
		bind_arrays();
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
		GLuint vertex = 0;
		GLuint normal = 0;
		GLuint color = 0;
		std::string sampler;
		int triangles_amount = 0;

		glm::vec3 light_color;
		GLfloat light_intensity;

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
			return collider_radius * length(scale) / sqrtf(3.0);
		}

		friend std::ofstream& operator<<(std::ofstream& ofs, const MyObject& obj) {
			ofs << obj.collider_radius << std::endl;
			ofs << obj.program << std::endl;
			ofs << obj.texture << std::endl;
			ofs << obj.vertex << std::endl;
			ofs << obj.normal << std::endl;
			ofs << obj.color << std::endl;
			ofs << "[" << obj.sampler << std::endl;
			ofs << obj.triangles_amount << std::endl;
			ofs << obj.light_color;
			ofs << obj.light_intensity << std::endl;
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
			ifs >> obj.vertex;
			ifs >> obj.normal;
			ifs >> obj.color;
			ifs >> obj.sampler; obj.sampler = obj.sampler.substr(1);
			ifs >> obj.triangles_amount;
			ifs >> obj.light_color;
			ifs >> obj.light_intensity;
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
	std::vector<MyObject> objects;
	std::vector<MyObject> uiobjects;
	std::vector<GLuint> shaders;
	std::vector<GLuint> textures;

	// actually no need to save these as they are recalculated before use, though save for now
	glm::mat4 projection;
	glm::mat4 view;
	std::vector<glm::vec3> point_light_positions;
	std::vector<glm::vec4> point_light_colors;
	std::vector<glm::vec3> directional_light_directions;
	std::vector<glm::vec4> directional_light_colors;
	std::vector<std::string> directional_light_labels;
	glm::vec3 sky_light_color;

	void recalculate_projection() {
		projection = glm::perspective(projection_angle, projection_ratio, projection_dist_min, projection_dist_max);
	}

	void recalculate_view() {
		view = glm::lookAt(camera_pos, camera_look, camera_head);
	}

	void recalculate_lights() {
		point_light_positions.clear();
		point_light_colors.clear();
		for (size_t obj = 0; obj != objects.size(); ++obj) {
			if (!objects[obj].is_deleted && objects[obj].light_intensity > 0) {
				point_light_positions.push_back(objects[obj].translation);
				point_light_colors.push_back(glm::vec4(objects[obj].light_color, objects[obj].light_intensity));
			}
		}
	}

	void update_single_object(const MyObject& obj) {
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
		glUniformMatrix4fv(glGetUniformLocation(obj.program, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(obj.program, "model_matrix"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(obj.program, "model_rotation_matrix"), 1, GL_FALSE, &model_rotation[0][0]);
		glUniform3fv(glGetUniformLocation(obj.program, "camera_pos"), 1, &camera_pos[0]);
		glUniform3fv(glGetUniformLocation(obj.program, "raw_camera_look"), 1, &camera_look[0]);
		glUniform1i(glGetUniformLocation(obj.program, "point_lights_amount"), point_light_colors.size());
		glUniform4fv(glGetUniformLocation(obj.program, "point_light_colors"), point_light_colors.size(), &point_light_colors[0][0]);
		glUniform3fv(glGetUniformLocation(obj.program, "point_light_positions"), point_light_positions.size(), &point_light_positions[0][0]);
		glUniform1i(glGetUniformLocation(obj.program, "directional_lights_amount"), directional_light_colors.size());
		glUniform4fv(glGetUniformLocation(obj.program, "directional_light_colors"), directional_light_colors.size(), &directional_light_colors[0][0]);
		glUniform3fv(glGetUniformLocation(obj.program, "directional_light_directions"), directional_light_directions.size(), &directional_light_directions[0][0]);
		glUniform3fv(glGetUniformLocation(obj.program, "sky_light_color"), 1, &sky_light_color[0]);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, obj.triangles_amount);
	}

	void update_single_uiobject(const MyObject& obj) {
		glm::mat4 model = obj.calculate_model_matrix();
		glm::mat4 MVP = projection * model;
		//MVP = projection * view * model;
		//MVP = projection * glm::lookAt(camera_pos, {-1.0f, 0.0f, 0.0f}, camera_head) * model;

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
		glUniformMatrix4fv(glGetUniformLocation(obj.program, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(obj.program, "model_matrix"), 1, GL_FALSE, &model[0][0]);

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
	}

	~MyScene() {
		// Cleanup
		for (auto& obj : objects) {
			glDeleteBuffers(1, &obj.vertex);
			glDeleteBuffers(1, &obj.color);
			glDeleteBuffers(1, &obj.normal);
			glDeleteProgram(obj.program);
			glDeleteTextures(1, &obj.texture);
		}
		glDeleteVertexArrays(1, &VertexArrayID);
		// Close OpenGL window and terminate GLFW
		glfwTerminate();
	}

	size_t amount_of_objects() const {
		return objects.size();
	}

	size_t amount_of_directional_lights() const {
		return directional_light_colors.size();
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
		glm::vec3 cur_forward = get_forward();
		glm::vec3 cur_head = camera_head;
		glm::vec3 cur_right = get_right();
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
								   	   (cwd + shader_path + ".fragmentshader").c_str());
		shaders.push_back(programID);
		return shaders.size() - 1;
	}

	[[nodiscard]] size_t load_texture(const std::string& texture_path) {
		GLuint textureID = loadBMP_custom((cwd + texture_path).c_str());
		textures.push_back(textureID);
		return textures.size() - 1;
	}

	// ONLY MAX_LIGHTS LIGHTS CAN BE AVALIABLE SIMULTANEOUSLY OTHERWISE UB
	void attach_point_light(size_t object, glm::vec3 color, GLfloat intensity) {
		objects[object].light_color = color;
		objects[object].light_intensity = intensity;
	}

	void add_directional_light(const glm::vec3& new_dir_light_direction, const glm::vec4& new_dir_light_color, const std::string& new_dir_light_label) {
		directional_light_colors.push_back(new_dir_light_color);
		directional_light_labels.push_back(new_dir_light_label);
		directional_light_directions.push_back(new_dir_light_direction);
	}

	void set_sky_light(const glm::vec3& new_sky_light_color) {
		sky_light_color = new_sky_light_color;
	}

	void remove_point_light(size_t object) {
		objects[object].light_color = glm::vec3(0);
		objects[object].light_intensity = 0;
	}

	size_t add_object(MyObjectRaw obj, std::string label) {
		MyObject new_object;
		new_object.program = shaders[obj.program_id];
		new_object.texture = textures[obj.texture_id];
		new_object.vertex = obj.vertexBufferID;
		new_object.color = obj.colorBufferID;
		new_object.normal = obj.normalBufferID;
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
		recalculate_lights();

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

		// Start of drawing UI things
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		for (auto& obj : uiobjects) {
			update_single_uiobject(obj);
		}
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		// End of drawing UI things

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		// Disable buffers with attributes

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void despawn_far() {
		for (size_t obj = 0; obj != objects.size(); ++obj) {
			if (glm::distance(objects[obj].translation, camera_pos) >= DESPAWN_DISTANCE_IN_MAX_DISTANCE * projection_dist_max) {
				remove_point_light(obj);
				objects[obj].is_deleted = true;
			}
		}
	}

	void clear_ui() {
		uiobjects.clear();
	}

	void add_ui(MyObjectRaw obj, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rot_axis, GLfloat rot_angle) {
		MyObject new_object;
		new_object.program = shaders[obj.program_id];
		new_object.texture = textures[obj.texture_id];
		new_object.vertex = obj.vertexBufferID;
		new_object.color = obj.colorBufferID;
		new_object.normal = obj.normalBufferID;
		new_object.is_texture_instead_of_color = obj.is_texture_instead_of_color;
		new_object.triangles_amount = obj.verts.size() / 3;

		new_object.translation = pos;
		new_object.rotation_angle = rot_angle;
		new_object.rotation_axis = rot_axis;
		new_object.scale = scale;

		uiobjects.push_back(new_object);
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
		file_to_save << objects << uiobjects;
		file_to_save << point_light_positions << point_light_colors;
		file_to_save << directional_light_directions << directional_light_colors;
		file_to_save << directional_light_labels;
		file_to_save << sky_light_color;
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
		file_to_load >> objects >> uiobjects;
		file_to_load >> point_light_positions >> point_light_colors;
		file_to_load >> directional_light_directions >> directional_light_colors;
		file_to_load >> directional_light_labels;
		file_to_load >> sky_light_color;
		file_to_load >> camera_pos >> camera_look >> camera_head >> projection >> view;
		file_to_load.close();
		printf("LOAD IS SUCCESFULL\n");
	}
};


std::vector<GLfloat> get_vert_sphere(GLfloat radius, int triangles_in_height, int triangles_in_width) {
	GLfloat angle_height = PI / triangles_in_height;
	GLfloat angle_width = 2 * PI / triangles_in_width;
	std::vector<GLfloat> result;
	std::vector<glm::vec3> lst_points(triangles_in_width + 1, {0, 0, radius});
	for (int k = 1; k <= triangles_in_height; ++k) {
		// calculate new points
		std::vector<glm::vec3> new_points;
		if (k == triangles_in_height) {
			new_points = std::vector<glm::vec3>(triangles_in_width + 1, {0, 0, -radius});
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

std::vector<GLfloat> get_color_simple(const std::vector<GLfloat>& vert, glm::vec3 color) {
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
   	getcwd(dir, sizeof(dir));
   	std::string cwd = dir;
	cwd += "/../homework02/";


	MyScene myScene(cwd);

	// configure initial scene position
	// View: Angle of view: 60, ratio: 1:1, display distance: 0.1 <-> 128
	myScene.projection_angle = glm::radians(60.0f);
	myScene.projection_ratio = SCREEN_WIDTH * 1.0f / SCREEN_HEIGHT;
	myScene.projection_dist_min = 0.1f;
	myScene.projection_dist_max = 128.0f;
	// Camera: pos(0, 0, 0), look(0, 1, 0), head(0, 1, 0)
	myScene.camera_pos = glm::vec3(0, 0, 0);
	myScene.camera_look = glm::vec3(1, 0, 0);
	myScene.camera_head = glm::vec3(0, 0, -1);


	// load shaders
	size_t shader_texture_basic = myScene.load_shader("shaders/TextureBasic");
	size_t shader_color_light = myScene.load_shader("shaders/ColorLight");
	size_t shader_fireball_hit = myScene.load_shader("shaders/FireballHit");
	size_t shader_color_ui = myScene.load_shader("shaders/ColorUI");

	// load textures
	size_t texture_uv_fireball = myScene.load_texture("textures/fireball.bmp");
	size_t texture_uv_sky = myScene.load_texture("textures/sky.bmp");


	// sky
	std::vector<GLfloat> sky_vert = get_vert_sphere(myScene.projection_dist_max - 1, 32, 64);
	std::vector<GLfloat> sky_uv = get_uv_sphere(sky_vert);
	MyObjectRaw sky_raw(sky_vert, false, myScene.projection_dist_max - 1);
	sky_raw.set_texture(texture_uv_sky, "textureSamplerSky", sky_uv, shader_texture_basic);

	// fireball
	std::vector<GLfloat> fireball_vert = get_vert_sphere(0.5, 16, 32);
	std::vector<GLfloat> fireball_uv = get_uv_sphere(fireball_vert);
	MyObjectRaw fireball_raw(fireball_vert, false, 0.5);
	fireball_raw.set_texture(texture_uv_fireball, "textureSamplerFireball", fireball_uv, shader_texture_basic);

	// fireball hit sphere
	std::vector<GLfloat> fireball_hit_vert = get_vert_sphere(0.25, 16, 32);
	std::vector<GLfloat> fireball_hit_col = get_color_simple(fireball_hit_vert, {1, 0.2, 0.2});
	MyObjectRaw fireball_hit_raw(fireball_hit_vert, true, 0.25);
	fireball_hit_raw.set_colors(fireball_hit_col, shader_fireball_hit);

	// enemy
	/*std::vector<GLfloat> pyramid_enemy_vert = vector_from_file(cwd + "models/pyramid.vert");
	std::vector<GLfloat> pyramid_enemy_col = vector_from_file(cwd + "models/pyramid.col");
	MyObjectRaw pyramid_enemy_raw(pyramid_enemy_vert, false, sqrtf(3) / 2);
	pyramid_enemy_raw.set_colors(pyramid_enemy_col, shader_color_light); //*/
	std::vector<GLfloat> pyramid_enemy_vert = get_vert_sphere(sqrtf(3), 64, 128);
	std::vector<GLfloat> pyramid_enemy_col = get_color_simple(pyramid_enemy_vert, {1, 0.5, 1});
	MyObjectRaw pyramid_enemy_raw(pyramid_enemy_vert, false, sqrtf(3));
	pyramid_enemy_raw.set_colors(pyramid_enemy_col, shader_color_light); //*/

	// digits
	std::vector<MyObjectRaw> digits_raw;
	for (int i = 0; i < 10; ++i) {
		std::vector<GLfloat> digit_vert = vector_from_file(cwd + "models/digits/digit" + (char)('0' + i) + ".vert");
		std::vector<GLfloat> digit_col = vector_from_file(cwd + "models/digits/digit" + (char)('0' + i) + ".col");
		MyObjectRaw digit_raw(digit_vert, false);
		digit_raw.set_colors(digit_col, shader_color_ui);
		digits_raw.push_back(digit_raw);
	}


	myScene.add_object(sky_raw, TYPE_SKY);
	myScene.set_sky_light(SKY_LIGHT_COLOR);
	myScene.add_directional_light(MOON_LIGHT_DIRECTION, MOON_LIGHT_COLOR, MOON_LIGHT_LABEL);
	myScene.add_object(fireball_hit_raw, TYPE_FIREBALL_HIT);
	int effect_fireball_hit = -1;

	long long ticks = 0;
	long long last_fps_ms = 0;
	long long enemy_last_spawn_time = 0;
	long long fireball_last_spawn_time = 0;
	long long last_frame_time = 0;
	bool save_button_was_pressed = false;
	bool load_button_was_pressed = false;
	bool object_movement_enabled = true;
	long long enemies_killed = 0;
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
		if (ticks < 5) { // to stabilize cursor
			mouse_x_delta = 0;
			mouse_y_delta = 0;
		} else {
			mouse_x_delta -= SCREEN_WIDTH / 2;
			mouse_y_delta -= SCREEN_HEIGHT / 2;
			mouse_x_delta *= MOUSE_SPEED;
			mouse_y_delta *= MOUSE_SPEED;
		}


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


		// delete objects
		myScene.despawn_far();
		// collide
		glm::vec3 boom;
		auto fireball = sphere_fireballs.begin();
		while (fireball != sphere_fireballs.end()) {
			auto enemy = pyramid_enemies.begin();
			while (enemy != pyramid_enemies.end()) {
				GLfloat dist = distance(myScene[*enemy].translation, myScene[*fireball].translation);
				GLfloat coll = myScene[*enemy].get_collider_radius() + myScene[*fireball].get_collider_radius();
				if (dist < coll) {
					boom = myScene[*fireball].translation;
					myScene[*fireball].is_deleted = true;
					myScene[*enemy].is_deleted = true;
					myScene.remove_point_light(*fireball);
					sphere_fireballs.erase(fireball++);
					pyramid_enemies.erase(enemy++);
					break;
				}
				++enemy;
			}
			if (boom != glm::vec3()) break;
			++fireball;
		}


		// create objects
		if (current_ms() - enemy_last_spawn_time >= ENEMY_SPAWN_DELTA) {
			size_t pyramid = myScene.add_object(pyramid_enemy_raw, TYPE_PYRAMID_ENEMY);
			myScene[pyramid].translation = glm::vec3(random(-1, 1) * ENEMY_SPAWN_RADIUS,
											    random(-1, 1) * ENEMY_SPAWN_RADIUS,
												random(-1, 1) * ENEMY_SPAWN_RADIUS) + myScene.camera_pos;
			myScene[pyramid].rotation_angle = random(-1, 1) * PI;
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
				myScene.attach_point_light(fireball, FIREBALL_LIGHT_COLOR, FIREBALL_LIGHT_INTENSITY);
			  	sphere_fireballs.push_back(fireball);
			  	fireball_last_spawn_time = current_ms();
			}
		}


		// change camera pos
		glm::vec3 delta_cam_pos;
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
		if (length(delta_cam_pos) > 0) {
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
		if (glfwGetKey(window, KEY_STOP_OBJECT_MOVEMENT) == GLFW_PRESS) object_movement_enabled = false;
		if (glfwGetKey(window, KEY_RESUME_OBJECT_MOVEMENT) == GLFW_PRESS) object_movement_enabled = true;
		if (object_movement_enabled) {
			for (size_t fireball : sphere_fireballs) {
				myScene[fireball].rotation_angle += FIREBALL_ROTATION_RATE * delta_time;
				myScene[fireball].translation += myScene[fireball].speed * delta_time;
			}
			for (size_t enemy : pyramid_enemies) {
				myScene[enemy].rotation_angle += PYRAMID_ENEMY_ROTATION_RATE * delta_time;
				myScene[enemy].translation += myScene[enemy].speed * delta_time;
			}
		}
		myScene[sky].translation = myScene.camera_pos;


		// Add UI
		myScene.clear_ui();
		std::string enemies_killed_str = std::to_string(enemies_killed);
		float digit_delta = 1.0f / (enemies_killed_str.size() + 1);
		float digit_scale_rate = digit_delta * DIGIT_SCALE_RATE;
		for (int k = 0; k < enemies_killed_str.size(); ++k) {
			int digit = (enemies_killed_str[k] - '0');
			glm::vec3 digit_pos{-1.0f + digit_delta * (k + 0.5f), 1.0f - digit_delta * 0.5f, 0.0f};
			glm::vec3 digit_scale(digit_scale_rate);
			glm::vec3 digit_rotation_axis{0.0f, 0.0f, 1.0f};
			GLfloat digit_rotation_angle = glm::sin(ticks * 0.01f) * 0.5f;
			myScene.add_ui(digits_raw[digit], digit_pos, digit_scale, digit_rotation_axis, digit_rotation_angle);
		}

		// Effects
		if (boom != glm::vec3()) {
			effect_fireball_hit = MAX_EFFECT_FIREBALL_HIT;
			myScene[fireball_hit].is_deleted = false;
			myScene[fireball_hit].translation = boom;
			++enemies_killed;
		}
		// Effect of fireball
		if (effect_fireball_hit >= 0) {
			--effect_fireball_hit;
			myScene[fireball_hit].scale = glm::vec3(glm::pow(10, 1 - effect_fireball_hit * 1.0f / MAX_EFFECT_FIREBALL_HIT));
		} else {
			myScene[fireball_hit].is_deleted = true;
		}


		myScene.update();


		bool new_load_button_was_pressed = (glfwGetKey(window, KEY_LOAD) == GLFW_PRESS);
		bool new_save_button_was_pressed = (glfwGetKey(window, KEY_SAVE) == GLFW_PRESS);
		// load scene
		if (load_button_was_pressed && !new_load_button_was_pressed) myScene.load(FILENAME_SAVE);
		// save scene
		if (save_button_was_pressed && !new_save_button_was_pressed) myScene.save(FILENAME_SAVE);
		// update buttons states
		load_button_was_pressed = new_load_button_was_pressed;
		save_button_was_pressed = new_save_button_was_pressed;
	} while(glfwGetKey(window, KEY_EXIT) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return 0;
}
