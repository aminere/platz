

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

unsigned char* pixels;
int stride;
bool pixelsDirty = true;
void setPixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b) {
	const auto index = (y * stride * 3) + x * 3;
	pixels[index + 0] = r;
	pixels[index + 1] = g;
	pixels[index + 2] = b;
	pixelsDirty = true;
}

int main(void) {
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	const auto monitor = glfwGetPrimaryMonitor();
	const auto mode = glfwGetVideoMode(monitor);

	window = glfwCreateWindow(1024, 768, "Platz", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH);

	const auto createShader = [](int type, const char* code) {
		const auto shader = glCreateShader(type);
		glShaderSource(shader, 1, &code, NULL);
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "glCompileShader Failed\n" << code << std::endl << infoLog << std::endl;
			return -1;
		}
		return (int)shader;
	};

	const char* vertexShaderStr = "#version 330 core\n"
		"in vec3 position;\n"
		"in vec2 uv;\n"
		"out vec2 vUv;\n"
		"void main()\n"
		"{\n"
		"   vUv = uv;\n"
		"   gl_Position = vec4(position, 1.0);\n"
		"}";

	const auto fragmentShaderStr = "#version 330 core\n"
		"in vec2 vUv;\n"
		"uniform sampler2D pixels;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"vec4 pixelColor = texture(pixels, vUv);\n"
		"fragColor = pixelColor;\n"
		"}\0";

	const auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderStr);
	const auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderStr);
	const auto shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "glLinkProgram Failed\n" << infoLog << std::endl;
		return -1;
	}

	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	const auto positionLocation = glGetAttribLocation(shaderProgram, "position");
	const auto uvLocation = glGetAttribLocation(shaderProgram, "uv");
	const auto pixelsLocation = glGetUniformLocation(shaderProgram, "pixels");

	// Vertex buffers
	float positions[] = {
		-1.f, -1.f, 0.0f,
		1.f, -1.f, 0.0f,
		-1.f,  1.f, 0.0f,
		1.f,  1.f, 0.0f
	};
	float uvs[] = {
		0.f, 1.f,
		1.f, 1.f,
		0.f, 0.f,
		1.f, 0.f
	};

	unsigned int positionsBuffer;
	glGenBuffers(1, &positionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	unsigned int uvsBuffer;
	glGenBuffers(1, &uvsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvLocation);
	glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	stride = width;
	const auto size = stride * height * 3;
	pixels = new unsigned char[size];
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < stride; ++j) {
			setPixel(j, i, 0, 0, 0);
		}
	}

	unsigned int texture;
	const auto textureState = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, stride, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glUniform1i(pixelsLocation, 0);	

	auto previousTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		//glClear(GL_COLOR_BUFFER_BIT);

		auto currentTime = glfwGetTime();
		const auto deltaTime = currentTime - previousTime;
		previousTime = currentTime;		

		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < stride; ++j) {
				setPixel(
					j,
					i,
					(unsigned char)((((float)j) / stride) * 255.f),
					(unsigned char)(((float)i / height) * 255.f),
					(unsigned char)(((float)(stride - j) / stride) * 255.f)
				);
			}
		}

		if (pixelsDirty) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, stride, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
			pixelsDirty = false;
		}

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
