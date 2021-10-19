

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
		auto material = std::make_shared<PhongMaterial>(Color::white * .1f, texture, 32.f);

		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one * 6.f)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(planeVb),
		//		material
		//	);

		auto plane = Entities::create()
			->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one * 10)
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
					{{1, 0, 1, 1}, { 1, 1 }, { 0, 1, 0 }, { 1, 0, 0, 1}},
					{{1, 0, -1, 1}, { 1, 0 }, { 0, 1, 0 }, { 0, 0, 1, 1}},
					{{-1, 0, 1, 1}, { 0, 1 }, { 0, 1, 0 }, { 0, 1, 0, 1}},
					{{-1, 0, 1, 1}, { 0, 1 }, { 0, 1, 0 }, { 0, 1, 0, 1}},
					{{1, 0, -1, 1}, { 1, 0 }, { 0, 1, 0 }, { 0, 0, 1, 1}},
					{{-1, 0, -1, 1}, { 0, 0 }, { 0, 1, 0 }, { 0, 1, 0, 1}},
					}))),
					material
				);

		plane->getComponent<Visual>()->castShadows = false;

		//Entities::create()
		//	->setComponent<Transform>(Vector3(0, 0, -2), Quaternion::identity, Vector3::one)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(cubeVb),
		//		material
		//	);

		auto sphere = Entities::create()
			->setComponent<Transform>(Vector3(0, 3, 2), Quaternion::identity, Vector3::one * 1.f)
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(sphereVb),
				material
				);
		sphere->getComponent<Visual>()->receiveShadows = false;
		sphere->getComponent<Visual>()->castShadows = false;

		auto cube = Entities::create()
			->setComponent<Transform>(Vector3(2, 1, 2), Quaternion::identity, Vector3::one * 1.f)
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(cubeVb),
				material
				);
		cube->getComponent<Visual>()->receiveShadows = false;
		cube->getComponent<Visual>()->castShadows = false;

		Entities::create()
			->setComponent<Transform>(Vector3(0, 4, 2), Quaternion::identity, Vector3::one)
			->setComponent<Light>();

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
		
		platz::Engine e(512, 512);

		e.onKeyChanged = [&](int key, int action) {
			if (key == GLFW_KEY_ESCAPE) {
				e.close();
				return;
			}
		
			const auto cameraSpeed = 10.f;
			if (action == GLFW_PRESS || action == GLFW_REPEAT) {

				auto deltaTime = e.deltaTime();
				auto cameraTransform = camera->getComponent<Transform>();
				auto cameraForward = cameraTransform->forward();
				auto cameraRight = cameraTransform->right();
				switch (key) {
				case GLFW_KEY_LEFT:
				case GLFW_KEY_A:
					cameraTransform->position(cameraTransform->position() - cameraRight * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_RIGHT:
				case GLFW_KEY_D:
					cameraTransform->position(cameraTransform->position() + cameraRight * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_UP:
				case GLFW_KEY_W:
					cameraTransform->position(cameraTransform->position() - cameraForward * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_DOWN:
				case GLFW_KEY_S:
					cameraTransform->position(cameraTransform->position() + cameraForward * cameraSpeed * deltaTime);
					break;
				}
			}
		};		

		bool _lookingStarted = false;
		Vector2 previousClickPos;
		e.onMouseDown = [&](const MouseInput& m) {
			if (m.left) {
				std::cout << "_lookingStarted = true" << std::endl;
				_lookingStarted = true;
				previousClickPos = Vector2(m.x, m.y);
			}
		};

		e.onMouseUp = [&](const MouseInput& m) {
			if (!m.left) {
				if (_lookingStarted) {
					_lookingStarted = false;
				}
			}			
		};

		e.onMouseMoved = [&](const MouseInput& m) {
			if (_lookingStarted) {
				auto cameraTransform = camera->getComponent<Transform>();
				auto clickPos = Vector2(m.x, m.y);
				auto deltaPos = clickPos - previousClickPos;
				previousClickPos = clickPos;
				auto lateralRotation = Quaternion(Vector3::up, -deltaPos.x * .01f);
				auto verticalRotation = Quaternion(cameraTransform->right(), -deltaPos.y * .01f);
				auto rotation = verticalRotation * lateralRotation;
				cameraTransform->rotation(cameraTransform->rotation() * rotation);				
			}
		};

		e.mainLoop();
	}

	return 0;
}
