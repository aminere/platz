#pragma once

#include "vector2.h"

namespace platz {

	class Canvas;

	class Renderer {
	public:

		static zmath::Vector2 screenSize;

		void render() const;

	private:

		std::unique_ptr<Canvas> _canvas;
	};
}
