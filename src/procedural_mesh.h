#pragma once

#include "geometry.h"

namespace platz {
	class ProceduralMesh : public Geometry {

		std::shared_ptr<Vertexbuffer> _vertexBuffer;

	public:

		ProceduralMesh(const std::shared_ptr<Vertexbuffer>& vb)
			: _vertexBuffer(vb)
		{
		}

		Vertexbuffer* getVertexBuffer() const override {
			return _vertexBuffer.get();
		}
	};
}
