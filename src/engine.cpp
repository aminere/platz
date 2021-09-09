
#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "canvas.h"
#include "components.h"
#include "visual.h"
#include "camera.h"
#include "transform.h"
#include "plane.h"

namespace platz {

	Engine* Engine::_instance = nullptr;

	Engine::Engine(int width, int height) {
		_instance = this;
		initCanvas(width, height);
		initFullscreenQuad();
	}

	void Engine::mainLoop() {
		auto previousTime = (float)glfwGetTime();
		while (!glfwWindowShouldClose(_window)) {

			_canvas->clear();

			auto currentTime = (float)glfwGetTime();
			_deltaTime = currentTime - previousTime;
			previousTime = currentTime;

			Components::extract();
			render();

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _canvas->width(), _canvas->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, _canvas->pixels());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}

	void Engine::render() {
		auto visuals = Components::ofType<Visual>();
		auto cameras = Components::ofType<Camera>();
		for (auto camera : cameras) {
			const auto& projectionView = camera->projector->getProjectionMatrix() * camera->getViewMatrix();
			auto cameraPos = camera->entity()->getComponent<Transform>()->position();
			for (auto visual : visuals) {

				auto transform = visual->entity()->getComponent<Transform>();
				auto mvp = projectionView * transform->getWorldMatrix();
				auto vb = visual->geometry->getVertexBuffer();
				auto material = visual->material.get();
				for (size_t i = 0; i < vb->vertices.size(); i += 3) {
					const auto& a = vb->vertices[i];
					const auto& b = vb->vertices[i + 1];
					const auto& c = vb->vertices[i + 2];
					
					// back face culling
					auto normal = zmath::Plane::fromPoints(a.position.xyz, b.position.xyz, c.position.xyz).normal;
					auto center = (a.position.xyz + b.position.xyz + c.position.xyz) / 3.f;
					if (normal.dot(cameraPos - center) < 0.f) {
						continue;
					}

					_canvas->drawTriangle(a, b, c, mvp, material);
				}
			}
		}
	}

	void Engine::close() {
		glfwSetWindowShouldClose(_window, GLFW_TRUE);
	}

	Engine::~Engine() {
		_instance = nullptr;
		glfwTerminate();
	}

	void Engine::onResize(int width, int height) {
		glViewport(0, 0, width, height);
		_canvas->onResize(width, height);
	}

	void Engine::initCanvas(int width, int height) {
		if (!glfwInit()) {
			throw "glfwInit failed";
		}

		_window = glfwCreateWindow(width, height, "Platz", NULL, NULL);
		if (!_window) {
			glfwTerminate();
			throw "glfwCreateWindow failed";
		}

		glfwSetWindowUserPointer(_window, this);
		glfwMakeContextCurrent(_window);
		glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* w, int x, int y) {
			static_cast<Engine*>(glfwGetWindowUserPointer(w))->onResize(x, y);
		});
		glfwSetKeyCallback(_window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
			static_cast<Engine*>(glfwGetWindowUserPointer(w))->onKeyChanged(key, action);
		});

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw "gladLoadGLLoader failed";
		}

		int canvasWidth, canvasHeight;
		glfwGetFramebufferSize(_window, &canvasWidth, &canvasHeight);
		_canvas = std::make_unique<Canvas>(canvasWidth, canvasHeight);		
	}

	void Engine::initFullscreenQuad() {
		glViewport(0, 0, _canvas->width(), _canvas->height());
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH);

		const auto vertexShaderStr = "#version 330 core\n"
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
			"}";

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
				throw "createShader failed";
			}
			return (int)shader;
		};
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
			throw "glLinkProgram Failed";
		}

		glUseProgram(shaderProgram);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		float positions[] = {
			-1.f, -1.f, 0.0f,
			1.f, -1.f, 0.0f,
			-1.f,  1.f, 0.0f,
			1.f,  1.f, 0.0f
		};
		unsigned int positionsBuffer;
		glGenBuffers(1, &positionsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		const auto positionLocation = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(positionLocation);
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		float uvs[] = {
			0.f, 1.f,
			1.f, 1.f,
			0.f, 0.f,
			1.f, 0.f
		};
		unsigned int uvsBuffer;
		glGenBuffers(1, &uvsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
		const auto uvLocation = glGetAttribLocation(shaderProgram, "uv");
		glEnableVertexAttribArray(uvLocation);
		glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _canvas->width(), _canvas->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, _canvas->pixels());
		const auto pixelsLocation = glGetUniformLocation(shaderProgram, "pixels");
		glUniform1i(pixelsLocation, 0);
	}
}

