

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
#include "light.h"
#include "texture.h"
#include "phong_material.h"

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
		std::shared_ptr<Vertexbuffer> sphereVb(OBJLoader::load("media/sphere.obj"));

		auto camera = Entities::create()
			->setComponent<Camera>(new PerspectiveProjector(60.f, 1.f, 100.f))
			->setComponent<Transform>(Vector3(0.f, 2.f, 10), Quaternion::identity, Vector3::one);

		auto texture = std::make_shared<Texture>("media/wood.png");
		auto material = std::make_shared<PhongMaterial>(Color::red);

		Entities::create()
			->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one * 6.f)
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(planeVb),
				material
			);

		Entities::create()
			->setComponent<Transform>(Vector3(0, 0, -2), Quaternion::identity, Vector3::one)
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(cubeVb),
				material
			);

		Entities::create()
			->setComponent<Transform>(Vector3(0, 2, 8), Quaternion::identity, Vector3::one * .2f)
			->setComponent<Light>()
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(sphereVb),
				material
			);

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

		////Entities::create()
		////	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one * 10)
		////	->setComponent<Visual>(
		////		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		////			{{0, 0, 0, 1}, { 0, 1 }, { 0, 0, 0 }, { 1, 0, 0, 1}},
		////			{{1, 0, -1, 1}, { 1, 0 }, { 0, 0, 0 }, { 0, 0, 1, 1}},
		////			{{0, 0, -1, 1}, { 0, 0 }, { 0, 0, 0 }, { 0, 1, 0, 1}},
		////			}))),
		////			material
		////			);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one * 20)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		//			{{0, 0, 0, 1}, { 0, 0 }, { 0, 0, 0 }, { 1, 0, 0, 1}},
		//			{{1, 0, 1, 1}, { 1, 1 }, { 0, 0, 0 }, { 0, 0, 1, 1}},
		//			{{1, 0, 0, 1}, { 1, 0 }, { 0, 0, 0 }, { 0, 1, 0, 1}},					
		//			}))),
		//			material
		//		);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one * 3)
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
