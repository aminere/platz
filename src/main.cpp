

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

#include <iostream>

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

struct Triangle {
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;

	bool contains(const glm::vec3& p, glm::vec3& coords) {
		auto _coords = getBarycentricCoords(p);
		auto tolerance = 0.0005f;
		auto inside = (_coords.y >= -tolerance)
			&& (_coords.z >= -tolerance)
			&& (_coords.y + _coords.z - tolerance <= 1.f);
		
		if (inside) {
			coords = _coords;
		}

		return inside;
	}

	glm::vec3 getBarycentricCoords(const glm::vec3& p) {		
		auto v0 = b - a;
		auto v1 = c - a;
		auto v2 = p - a;
		auto d00 = glm::dot(v0, v0);
		auto d01 = glm::dot(v0, v1);
		auto d11 = glm::dot(v1, v1);
		auto d20 = glm::dot(v2, v0);
		auto d21 = glm::dot(v2, v1);
		auto det = (d00 * d11 - d01 * d01);
		if (det != 0) {
			auto v = (d11 * d20 - d01 * d21) / det;
			auto w = (d00 * d21 - d01 * d20) / det;
			auto u = 1.0f - v - w;
			return glm::vec3(u, v, w);
		}
		else {
			return glm::vec3(0.f, 999999.f, 999999.f);
		}
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

Color colorA = { 0xff, 0, 0 };
Color colorB = { 0, 0xff, 0 };
Color colorC = { 0, 0, 0xff };
void drawTriangle(const Context& context, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const Color& color) {
	const auto minX = fminf(a.x, fminf(b.x, c.x));
	const auto minY = fminf(a.y, fminf(b.y, c.y));
	const auto maxX = fmaxf(a.x, fmaxf(b.x, c.x));
	const auto maxY = fmaxf(a.y, fmaxf(b.y, c.y));
	glm::vec3 coords;
	Triangle triangle = { a, b, c };
	for (auto i = minY; i < maxY; ++i) {
		for (auto j = minY; j < maxY; ++j) {
			if (triangle.contains(glm::vec3(j, i, 0), coords)) {
				auto r = glm::min(coords.x * colorA.r + coords.y * colorB.r + coords.z * colorC.r, (float)0xff);
				auto g = glm::min(coords.x * colorA.g + coords.y * colorB.g + coords.z * colorC.g, (float)0xff);
				auto b = glm::min(coords.x * colorA.b + coords.y * colorB.b + coords.z * colorC.b, (float)0xff);				
				Color c = { (unsigned int)r, (unsigned int)g, (unsigned int)b };
				drawPixel(context, (unsigned int)j, (unsigned int)i, c);
			}
		}
	}
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
		std::cout << "Failed to initialize OpenGL context" << std::endl;
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

	glm::vec3 a(100, 100, 0);
	glm::vec3 b(200, 100, 0);
	glm::vec3 c(100, 200, 0);
	Color color = { 0xff, 0xff, 0xff };

	auto previousTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {

		const auto size = drawContext.width * drawContext.height * drawContext.bytesPerPixel;
		memset(drawContext.pixels, 0, size);

		auto currentTime = glfwGetTime();
		const auto deltaTime = currentTime - previousTime;
		previousTime = currentTime;

		//for (int i = 0; i < height; ++i) {
		//	for (int j = 0; j < width; ++j) {
		//		setPixel(
		//			drawContext,
		//			j,
		//			i,
		//			{ (unsigned char)((((float)j) / width) * 255.f),
		//			(unsigned char)(((float)i / height) * 255.f),
		//			(unsigned char)(((float)(width - j) / width) * 255.f) }
		//		);
		//	}
		//}

		drawLine(drawContext, a, b, color);
		drawLine(drawContext, b, c, color);
		drawLine(drawContext, a, c, color);
		drawTriangle(drawContext, a, b, c, color);
		
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
