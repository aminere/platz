

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

#include "nmmintrin.h"

using namespace platz;
using namespace zmath;

int main(void) {	

	{
		std::shared_ptr<Vertexbuffer> cubeVb(OBJLoader::load("media/cube.obj"));
		std::shared_ptr<Vertexbuffer> planeVb(OBJLoader::load("media/plane.obj"));
		std::shared_ptr<Vertexbuffer> sphereVb(OBJLoader::load("media/sphere.obj"));
		std::shared_ptr<Vertexbuffer> bunnyVb(OBJLoader::load("media/bunny.obj"));

		auto camera = Entities::create()
			->setComponent<Camera>(new PerspectiveProjector(60.f, 1.f, 100.f))
			->setComponent<Transform>(Vector3(0, 1, 4), Quaternion(Vector3(zmath::radians(-10), 0, 0)), Vector3::one);

		auto woodTex = std::make_shared<Texture>("media/crate.png");
		auto metalTex = std::make_shared<Texture>("media/metal.png");
		auto checkerTex = std::make_shared<Texture>("media/grass.png");
		auto woodMat = std::make_shared<PhongMaterial>(Color(0, .05, 0, 1), metalTex, 32.f);
		auto checkerMat = std::make_shared<PhongMaterial>(Color::white * .1f, checkerTex, 32.f);

		auto plane = Entities::create()
			->setComponent<Transform>(Vector3(0, -1, 0), Quaternion::identity, Vector3::one * 4)
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
					{{1, 0, 1, 1}, { 1, 1 }, { 0, 1, 0 }, { 1, 0, 0, 1}},
					{{1, 0, -1, 1}, { 1, 0 }, { 0, 1, 0 }, { 0, 0, 1, 1}},
					{{-1, 0, 1, 1}, { 0, 1 }, { 0, 1, 0 }, { 0, 1, 0, 1}},
					{{-1, 0, 1, 1}, { 0, 1 }, { 0, 1, 0 }, { 0, 1, 0, 1}},
					{{1, 0, -1, 1}, { 1, 0 }, { 0, 1, 0 }, { 0, 0, 1, 1}},
					{{-1, 0, -1, 1}, { 0, 0 }, { 0, 1, 0 }, { 0, 1, 0, 1}},
					}))),
					checkerMat
				);
		plane->getComponent<Visual>()->castShadows = false;
		plane->getComponent<Visual>()->receiveShadows = true;

		auto sphere = Entities::create()
			->setComponent<Transform>(Vector3(0, 0, 0), Quaternion::identity, Vector3::one * 1.f)
			->setComponent<Visual>(
				std::make_shared<ProceduralMesh>(sphereVb),
				woodMat
				);
		sphere->getComponent<Visual>()->receiveShadows = false;
		sphere->getComponent<Visual>()->castShadows = true;

		//auto cube = Entities::create()
		//	->setComponent<Transform>(Vector3(0, 0, 0), Quaternion::identity, Vector3::one * 1.f)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(cubeVb),
		//		woodMat
		//		);
		//cube->getComponent<Visual>()->receiveShadows = false;
		//cube->getComponent<Visual>()->castShadows = true;

		//auto bunny = Entities::create()
		//	->setComponent<Transform>(Vector3(0, 0, 0), Quaternion::identity, Vector3::one * 20.f)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(bunnyVb),
		//		checkerMat
		//		);
		//bunny->getComponent<Visual>()->receiveShadows = false;
		//bunny->getComponent<Visual>()->castShadows = false;

		Entities::create()
			->setComponent<Transform>(Vector3(0, 0, 0), Quaternion(Vector3(zmath::radians(90), 0, 0)), Vector3::one)
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

		// Origin aligned triangle
		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		//			{{0, 0, 0, 1}, { 0, 1 }, { 0, 0, 1 }, { 1, 0, 0, 1}},
		//			{{1, 0, 0, 1}, { 1, 1 }, { 0, 0, 1 }, { 0, 0, 1, 1}},
		//			{{0, 1, 0, 1}, { 0, 0 }, { 0, 0, 1 }, { 0, 1, 0, 1}}
		//		}))),
		//		material
		//		);

		// CENTERED TRIANGLE
		//Entities::create()
		//	->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
		//	->setComponent<Visual>(
		//		std::make_shared<ProceduralMesh>(std::make_shared<Vertexbuffer>(std::vector<Vertex>({
		//			{{-.5, -0.5, -5, 1}, { 0, 1 }, { 0, 0, 1 }, { 1, 0, 0, 1}},
		//			{{.5, -.5, -5, 1}, { 1, 1 }, { 0, 0, 1 }, { 0, 1, 0, 1}},
		//			{{0, .5, -5, 1}, { 0, 0 }, { 0, 0, 1 }, { 0, 0, 1, 1}}
		//			}))),
		//		woodMat
		//		);		
		
		
		platz::Engine e(512, 512, 1);

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
				auto cameraUp = cameraTransform->up();
				switch (key) {
				case GLFW_KEY_A:
					cameraTransform->position(cameraTransform->position() - cameraRight * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_D:
					cameraTransform->position(cameraTransform->position() + cameraRight * cameraSpeed * deltaTime);
					break;
				
				case GLFW_KEY_W:
					cameraTransform->position(cameraTransform->position() - cameraForward * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_S:
					cameraTransform->position(cameraTransform->position() + cameraForward * cameraSpeed * deltaTime);
					break;

				case GLFW_KEY_UP:
					cameraTransform->position(cameraTransform->position() + cameraUp * cameraSpeed * deltaTime);
					break;
				case GLFW_KEY_DOWN:
					cameraTransform->position(cameraTransform->position() - cameraUp * cameraSpeed * deltaTime);
					break;
				}
			}
		};		

		bool _lookingStarted = false;
		Vector2 previousClickPos;
		e.onMouseDown = [&](const MouseInput& m) {
			if (m.left) {
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
