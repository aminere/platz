

#include "pch.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <zmath.h>

#include "engine.h"
#include "entities.h"
#include "transform.h"
#include "camera.h"
#include "visual.h"
#include "perspective_projector.h"
#include "procedural_mesh.h"

#include "obj_loader.h"

#include "plane.h"
#include "triangle.h"
#include "clipping.h"

using namespace platz;
using namespace zmath;

//int imageWidth;
//int imageHeight;
//int imageChannels;
//unsigned char* imageData;

int main(void) {
	{
		std::shared_ptr<Vertexbuffer> cubeVb(OBJLoader::load("media/cube.obj"));
		std::shared_ptr<Vertexbuffer> planeVb(OBJLoader::load("media/plane.obj"));

		auto camera = Entities::create()
			->setComponent<Camera>(new PerspectiveProjector(60.f, .1f, 100.f))
			->setComponent<Transform>(Vector3(0.f, 2.f, 4), Quaternion::identity, Vector3::one);

		auto texture = std::make_shared<Texture>("media/checker.png");
		auto material = std::make_shared<Material>(texture);

		//Plane p(Vector3::right, Vector3(-2, 0, 0));
		//Triangle t(Vector3(0, 0, 0), Vector3(-3, 0, 0), Vector3(-3, 0, -1));
		//std::vector<Triangle> result;
		//auto clip = Clipping::trianglePlane(t, p, result);

		Entities::create()
			->setComponent<Transform>(Vector3(0, 0, -10), Quaternion::identity, Vector3::one)
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(cubeVb),
				material
			);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one * 1.f)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(planeVb),
		//		material
		//		);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		//			{{0, 1, 0, 1}, { 0, 1 }, { 0, 0, 0 }, { 1, 0, 0, 1}},
		//			{{1, 1, -1, 1}, { 1, 0 }, { 0, 0, 0 }, { 0, 0, 1, 1}},
		//			{{0, 1, -1, 1}, { 0, 0 }, { 0, 0, 0 }, { 0, 1, 0, 1}},					
		//			}))),
		//			material
		//		);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one * 4)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		//			{{0, 0, 0, 1}, { 0, 1 }, { 0, 0, 0 }, { 1, 0, 0, 1}},
		//			{{1, 0, -1, 1}, { 1, 0 }, { 0, 0, 0 }, { 0, 0, 1, 1}},
		//			{{0, 0, -1, 1}, { 0, 0 }, { 0, 0, 0 }, { 0, 1, 0, 1}},
		//			}))),
		//			material
		//			);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		//			{{0, 0, 0, 1}, { 0, 0 }, { 0, 0, 0 }, { 1, 0, 0, 1}},
		//			{{0, 0, 1, 1}, { 0, 1 }, { 0, 0, 0 }, { 0, 0, 1, 1}},
		//			{{1, 0, 1, 1}, { 1, 1 }, { 0, 0, 0 }, { 0, 1, 0, 1}},					
		//			}))),
		//			material
		//		);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		//			{{0, 0, 0, 1}, { 0, 1 }, { 0, 0, 0 }, { 1, 0, 0, 1}},
		//			{{1, 0, 0, 1}, { 1, 1 }, { 0, 0, 0 }, { 0, 0, 1, 1}},
		//			{{0, 1, 0, 1}, { 0, 0 }, { 0, 0, 0 }, { 0, 1, 0, 1}}
		//		}))),
		//		material
		//	);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		//			{{0, 0, 1, 0}, { 0, 1 }, { 1, 0, 0, 1}},
		//			{{1, 0, 0, 0}, { 1, 0 }, { 0, 1, 0, 1}},
		//			{{1, 0, 1, 0}, { 1, 1 }, { 0, 0, 1, 1}}
		//		}))),
		//		material
		//	);

		auto cameraForward = Vector3::forward;
		auto cameraRight = Vector3::right;
		auto cameraUp = Vector3::up;
		float cameraAngle = 0.f;

		platz::Engine e(1024, 768);

		e.onKeyChanged = [&](int key, int action) {
			if (key == GLFW_KEY_ESCAPE) {
				e.close();
				return;
			}

			const auto cameraSpeed = 10.f;
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {

				auto deltaTime = e.deltaTime();
				auto cameraTransform = camera->getComponent<Transform>();
				switch (key) {
				case GLFW_KEY_LEFT:
					cameraTransform->position(cameraTransform->position() + cameraRight * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_RIGHT:
					cameraTransform->position(cameraTransform->position() - cameraRight * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_UP:
					cameraTransform->position(cameraTransform->position() - cameraForward * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_DOWN:
					cameraTransform->position(cameraTransform->position() + cameraForward * cameraSpeed * deltaTime);
					break;

				case GLFW_KEY_B:
					cameraTransform->position(cameraTransform->position() + cameraUp * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_N:
					cameraTransform->position(cameraTransform->position() - cameraUp * cameraSpeed * deltaTime);
					break;

				case GLFW_KEY_C:
					cameraAngle -= deltaTime * 90.f;
					cameraTransform->rotation(Quaternion(Vector3(0.f, zmath::radians(cameraAngle), 0.f)));
					cameraForward = Quaternion(Vector3(0.f, zmath::radians(cameraAngle), 0.f)) * Vector3::forward;
					cameraRight = Quaternion(Vector3(0.f, zmath::radians(cameraAngle), 0.f)) * Vector3::right;
					break;
				case GLFW_KEY_V:
					cameraAngle += deltaTime * 90.f;
					auto q = Quaternion(Vector3(0.f, zmath::radians(cameraAngle), 0.f));
					cameraTransform->rotation(q);
					cameraForward = Quaternion(Vector3(0.f, zmath::radians(cameraAngle), 0.f)) * Vector3::forward;
					cameraRight = Quaternion(Vector3(0.f, zmath::radians(cameraAngle), 0.f)) * Vector3::right;
					break;
				}
			}
		};		

		e.mainLoop();
	}

	return 0;
}
