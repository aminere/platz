#pragma once

#include <functional>
#include "mouse_input.h"

struct GLFWwindow;

namespace platz {

	class Canvas;	

	class Engine {
	public:

		inline static Engine* instance() { return _instance; }

		Engine(int width, int height, int downscale = 1);
		~Engine();

		void mainLoop();
		void close();

		inline Canvas* canvas() const { return _canvas.get(); }
		inline float deltaTime() const { return _deltaTime; }

		std::function<void(float)> onUpdate = [](float f) {};

		std::function<void(int, int)> onKeyChanged;
		std::function<void(const MouseInput&)> onMouseDown;
		std::function<void(const MouseInput&)> onMouseMoved;
		std::function<void(const MouseInput&)> onMouseUp;

	private:

		static Engine* _instance;

		void render();
		void onResize(int width, int height);

		void initCanvas(int width, int height);
		void initFullscreenQuad();

		GLFWwindow* _window = nullptr;
		float _deltaTime = 0.f;
		int _downscale;
		unsigned int _texture;
		unsigned int _shaderProgram;
		std::unique_ptr<Canvas> _canvas;
		MouseInput _mouseInput;
	};
}
