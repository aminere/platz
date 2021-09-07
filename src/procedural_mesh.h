#pragma once

#include "geometry.h"

namespace platz {
	class ProceduralMesh : public Geometry {

		std::unique_ptr<Vertexbuffer> _vertexBuffer;

	public:

		ProceduralMesh(Vertexbuffer* vb)
			: _vertexBuffer(vb)
		{
		}

		Vertexbuffer* getVertexBuffer() const override {
			return _vertexBuffer.get();
		}
	};
}
