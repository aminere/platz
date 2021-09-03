

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/transform.hpp>

#include <matrix44.h>

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct Context {
	unsigned char* pixels;
	int width;
	int height;
	int bytesPerPixel;
};

struct Vertex {
	Vector3 position;
	Color color;
};

struct Triangle {
	Vertex a;
	Vertex b;
	Vertex c;

	bool contains(const Vector3& p, Vector3& coords) const {
		if (getBarycentricCoords(p, coords)) {
			if (coords.y() < 0) {
				return false;
			}
			if (coords.z() < 0) {
				return false;
			}
			if (coords.y() + coords.z() > 1.f) {
				return false;
			}			
			return true;
		}
		else {
			return false;
		}
	}

	bool getBarycentricCoords(const Vector3& p, Vector3& out) const {
		auto v0 = b.position - a.position;
		auto v1 = c.position - a.position;
		auto v2 = p - a.position;
		auto d00 = v0.dot(v0);
		auto d01 = v0.dot(v1);
		auto d11 = v1.dot(v1);
		auto d20 = v2.dot(v0);
		auto d21 = v2.dot(v1);
		auto det = (d00 * d11 - d01 * d01);
		if (det != 0.f) {
			auto v = (d11 * d20 - d01 * d21) / det;
			auto w = (d00 * d21 - d01 * d20) / det;
			auto u = 1.0f - v - w;
			out.set(u, v, w);
			return true;
		}
		return false;
	}
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

Context drawContext;
void resizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);

	const auto size = width * height * drawContext.bytesPerPixel;
	const auto pixels = new unsigned char[size];
	drawContext.width = width;
	drawContext.height = height;
	drawContext.pixels = pixels;
}

void drawPixel(const Context& context, unsigned int x, unsigned int y, const Color& color) {
	const auto stride = context.width * context.bytesPerPixel;
	const auto index = (y * stride) + x * context.bytesPerPixel;
	context.pixels[index + 0] = color.r;
	context.pixels[index + 1] = color.g;
	context.pixels[index + 2] = color.b;
}

void drawLine(const Context& context, float x0, float y0, float x1, float y1, const Color& color) {
	// Based on https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	const auto dx = abs(x1 - x0);
	const auto sx = x0 < x1 ? 1 : -1;
	const auto dy = -abs(y1 - y0);
	const auto sy = y0 < y1 ? 1 : -1;
	auto err = dx + dy;
	while (true) {
		drawPixel(context, (unsigned int)x0, (unsigned int)y0, color);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		const auto e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void drawLine(const Context& context, const glm::vec3& a, const glm::vec3& b, const Color& color) {
	drawLine(context, a.x, a.y, b.x, b.y, color);
}

//glm::vec3 screenPos(const glm::vec3& worldPos) {
//	glm::mat4 Projection = glm::perspective(glm::radians(65.0f), 1.f, 0.1f, 100.0f);
//	glm::mat4 View = glm::lookAt(
//		glm::vec3(0, 0, 20),
//		glm::vec3(0, 0, 0),
//		glm::vec3(0, 1, 0)
//	);
//	glm::mat4 Model = glm::mat4(1.0f);
//	glm::mat4 mvp = /*Projection * View **/ Model;
//	auto ndc = mvp * glm::vec4(worldPos, 0.f);
//	ndc.x = glm::clamp(ndc.x, -1.f, 1.f);
//	ndc.y = glm::clamp(ndc.y, -1.f, 1.f);
//	ndc.z = glm::clamp(ndc.z, -1.f, 1.f);
//	return glm::vec3((ndc.x + 1.f) / 2.f * drawContext.width, (ndc.y + 1.f) / 2.f * drawContext.height, ndc.z);
//}

Vector3 screenPos(const Vector3& worldPos) {
	const auto ratio = (float)drawContext.width / drawContext.height;
	const auto projectionMatrix = Matrix44::makePerspective(glm::radians(65.f), ratio, .1f, 100.f);
	Matrix44 cameraMatrix;
	cameraMatrix.compose(Vector3::create(0.f, 1.f, 10.f), Quaternion::identity, Vector3::one);
	Matrix44 viewMatrix;
	cameraMatrix.getInverse(viewMatrix);
	const auto mvp = projectionMatrix * viewMatrix;
	const auto ndc = mvp * worldPos;
	return Vector3::create(
		(ndc.x() + 1.f) / 2.f * drawContext.width,
		(ndc.y() + 1.f) / 2.f * drawContext.height,
		ndc.z()
	);
}

void drawTriangle(const Context& context, const Triangle& triangle) {
	const auto a = triangle.a;
	const auto b = triangle.b;
	const auto c = triangle.c;
	const auto minX = fminf(a.position.x(), fminf(b.position.x(), c.position.x()));
	const auto minY = fminf(a.position.y(), fminf(b.position.y(), c.position.y()));
	const auto maxX = fmaxf(a.position.x(), fmaxf(b.position.x(), c.position.x()));
	const auto maxY = fmaxf(a.position.y(), fmaxf(b.position.y(), c.position.y()));
	Vector3 coords;
	for (auto i = minY; i < maxY; ++i) {
		for (auto j = minX; j < maxX; ++j) {
			if (triangle.contains(Vector3::create(j, i, 0), coords)) {
				auto _r = coords.x() * a.color.r + coords.y() * b.color.r + coords.z() * c.color.r;
				auto _g = coords.x() * a.color.g + coords.y() * b.color.g + coords.z() * c.color.g;
				auto _b = coords.x() * a.color.b + coords.y() * b.color.b + coords.z() * c.color.b;
				Color c = { (unsigned char)_r, (unsigned char)_g, (unsigned char)_b };
				drawPixel(context, (unsigned int)j, (unsigned int)i, c);
			}
		}
	}
}

void drawTriangle(const Context& context, const Vertex& a, const Vertex& b, const Vertex& c) {
	Triangle screenTriangle = {
		{ screenPos(a.position), a.color },
		{ screenPos(b.position), b.color },
		{ screenPos(c.position), c.color }
	};

	drawTriangle(context, screenTriangle);
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
	glfwSetKeyCallback(window, keyCallback);
	glfwSetFramebufferSizeCallback(window, resizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		//std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	glfwGetFramebufferSize(window, &drawContext.width, &drawContext.height);
	glViewport(0, 0, drawContext.width, drawContext.height);
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
			//std::cout << "glCompileShader Failed\n" << code << std::endl << infoLog << std::endl;
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
		//std::cout << "glLinkProgram Failed\n" << infoLog << std::endl;
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

	drawContext.bytesPerPixel = 3;
	drawContext.pixels = new unsigned char[drawContext.width * drawContext.height * drawContext.bytesPerPixel];
	auto pixelsDirty = true;

	unsigned int texture;
	const auto textureState = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, drawContext.width, drawContext.height, 0, GL_RGB, GL_UNSIGNED_BYTE, drawContext.pixels);
	glUniform1i(pixelsLocation, 0);

	Vertex a = { {0, 0, 0}, { 0xff, 0, 0} };
	Vertex b = { {1, 0, 0}, { 0, 0xff, 0} };
	Vertex c = { {1, -1, 0}, { 0, 0, 0xff} };
	Color color = { 0xff, 0xff, 0xff };

	auto previousTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {

		const auto size = drawContext.width * drawContext.height * drawContext.bytesPerPixel;
		memset(drawContext.pixels, 0, size);

		auto currentTime = glfwGetTime();
		const auto deltaTime = currentTime - previousTime;
		previousTime = currentTime;		

		drawTriangle(drawContext, a, b, c);
		//drawLine(drawContext, a.position, b.position, color);
		//drawLine(drawContext, b.position, c.position, color);
		//drawLine(drawContext, a.position, c.position, color);

		pixelsDirty = true;

		if (pixelsDirty) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, drawContext.width, drawContext.height, 0, GL_RGB, GL_UNSIGNED_BYTE, drawContext.pixels);
			pixelsDirty = false;
		}

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
