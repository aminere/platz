#pragma once

#include <functional>

struct GLFWwindow;

namespace platz {

	class Canvas;	

	class Engine {
	public:

		inline static Engine* instance() { return _instance; }

		Engine(
			int width, 
			int height, 
			bool fullscreen
		);

		~Engine();

		void update();

		inline Canvas* canvas() const { return _canvas.get(); }

	private:

		static Engine* _instance;

		void render();
		void onResize(int width, int height);

		GLFWwindow* _window;
		std::unique_ptr<Canvas> _canvas;
	};
}
