

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/transform.hpp>

#include <matrix44.h>
#include <zmath.h>

#include <functional>

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
		} else {
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

Vector3 cameraForward;
Vector3 cameraRight;
Vector3 cameraPosition = Vector3::create(0.f, 0.f, 10.f);
float cameraAngle = 0.f;
float deltaTime = 0.f;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {	

	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	const auto cameraSpeed = 20.f;
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_LEFT:
			cameraPosition = cameraPosition + cameraRight * cameraSpeed * deltaTime;
			break;
		case GLFW_KEY_RIGHT:
			cameraPosition = cameraPosition - cameraRight * cameraSpeed * deltaTime;
			break;
		case GLFW_KEY_UP:
			cameraPosition = cameraPosition - cameraForward * cameraSpeed * deltaTime;
			break;
		case GLFW_KEY_DOWN:
			cameraPosition = cameraPosition + cameraForward * cameraSpeed * deltaTime;
			break;

		case GLFW_KEY_C:
			cameraAngle -= deltaTime * 90.f;
			cameraForward = Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f) * Vector3::forward;
			cameraRight = Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f) * Vector3::right;
			break;
		case GLFW_KEY_V:
			cameraAngle += deltaTime * 90.f;
			cameraForward = Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f) * Vector3::forward;
			cameraRight = Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f) * Vector3::right;
			break;
		}
	}
	
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

void drawPixel(const Context& context, int x, int y, const Color& color) {
	if (x < 0 || y < 0 || x >= context.width || y >= context.height) {
		return;
	}
	const auto stride = context.width * context.bytesPerPixel;
	const auto index = (y * stride) + x * context.bytesPerPixel;
	context.pixels[index + 0] = color.r;
	context.pixels[index + 1] = color.g;
	context.pixels[index + 2] = color.b;
}

void drawLine(const Context& context, float x0, float y0, float x1, float y1, const Color& color) {
	// Based on https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	auto ix1 = (int)x1;
	auto iy1 = (int)y1;
	auto ix0 = (int)x0;
	auto iy0 = (int)y0;
	const auto dx = abs(ix1 - ix0);
	const auto sx = ix0 < ix1 ? 1 : -1;
	const auto dy = -abs(iy1 - iy0);
	const auto sy = iy0 < iy1 ? 1 : -1;
	auto err = dx + dy;
	while (true) {
		drawPixel(context, ix0, iy0, color);
		if (ix0 == ix1 && iy0 == iy1) {
			break;
		}
		const auto e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			ix0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			iy0 += sy;
		}
	}
}

void drawLine(const Context& context, const Vector3& a, const Vector3& b, const Color& color) {
	drawLine(context, a.x(), a.y(), b.x(), b.y(), color);
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
				drawPixel(context, (int)j, (int)i, c);
			}
		}
	}
}

void drawTriangle(
	const Context& context,
	const Vertex& a,
	const Vertex& b,
	const Vertex& c,
	std::function<Vector3(const Vector3&)> project
) {
	Triangle screenTriangle = {
		{ project(a.position), a.color },
		{ project(b.position), b.color },
		{ project(c.position), c.color }
	};

	drawTriangle(context, screenTriangle);

	Color white = { 0xff, 0xff, 0xff };
	drawLine(context, screenTriangle.a.position, screenTriangle.b.position, white);
	drawLine(context, screenTriangle.b.position, screenTriangle.c.position, white);
	drawLine(context, screenTriangle.a.position, screenTriangle.c.position, white);
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

	auto previousTime = (float)glfwGetTime();

	const auto ratio = (float)drawContext.width / drawContext.height;
	const auto projectionMatrix = Matrix44::makePerspective(glm::radians(65.f), ratio, .1f, 100.f);
	cameraForward = Vector3::forward;
	cameraRight = Vector3::right;

	while (!glfwWindowShouldClose(window)) {

		const auto size = drawContext.width * drawContext.height * drawContext.bytesPerPixel;
		memset(drawContext.pixels, 0, size);

		auto currentTime = (float)glfwGetTime();
		deltaTime = currentTime - previousTime;
		previousTime = currentTime;
		
		drawTriangle(
			drawContext,
			a,
			b,
			c,
			[&](const Vector3& worldPos) {
				Matrix44 cameraMatrix;
				cameraMatrix.compose(
					cameraPosition, 
					Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f),
					Vector3::one
				);
				Matrix44 viewMatrix;
				cameraMatrix.getInverse(viewMatrix);
				const auto mvp = projectionMatrix * viewMatrix;
				auto ndc = mvp * worldPos;
				//ndc.x(glm::clamp(ndc.x(), -1.f, 1.f));
				//ndc.y(glm::clamp(ndc.y(), -1.f, 1.f));
				//ndc.z(glm::clamp(ndc.z(), -1.f, 1.f));
				return Vector3::create(
					(ndc.x() + 1.f) / 2.f * drawContext.width,
					(ndc.y() + 1.f) / 2.f * drawContext.height,
					ndc.z()
				);
			}
		);

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
