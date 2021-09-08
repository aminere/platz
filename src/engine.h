#pragma once

#include <functional>

struct GLFWwindow;

namespace platz {

	class Canvas;	

	class Engine {
	public:

		inline static Engine* instance() { return _instance; }

		Engine(int width, int height);
		~Engine();

		void mainLoop();
		void close();

		inline Canvas* canvas() const { return _canvas.get(); }
		inline float deltaTime() const { return _deltaTime; }

		std::function<void(int, int)> onKeyChanged;

	private:

		static Engine* _instance;

		void render();
		void onResize(int width, int height);

		void initCanvas(int width, int height);
		void initFullscreenQuad();

		GLFWwindow* _window = nullptr;
		float _deltaTime = 0.f;
		std::unique_ptr<Canvas> _canvas;
	};
}
