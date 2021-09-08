

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

using namespace platz;
using namespace zmath;

//int imageWidth;
//int imageHeight;
//int imageChannels;
//unsigned char* imageData;

int main(void) {
	{
		auto camera = Entities::create()
			->setComponent<Camera>(new PerspectiveProjector(60.f, .1f, 100.f))
			->setComponent<Transform>(Vector3::create(0.f, 1.f, 3.f), Quaternion::identity, Vector3::one);

		Entities::create()
			->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
			->setComponent<Visual>(
				new ProceduralMesh(new Vertexbuffer({
					{{0, 0, 0, 0}, { 0, 0 }, { 1, 0, 0, 1}},
					{{1, 1, 0, 0}, { 1, 1 }, { 0, 1, 0, 1}},
					{{1, 0, 0, 0}, { 1, 0 }, { 0, 0, 1, 1}}
				})),
				new Material()
			);

		Entities::create()
			->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
			->setComponent<Visual>(
				new ProceduralMesh(new Vertexbuffer({
					{{0.5, 0, -1, 0}, { 0, 0 }, { 1, 0, 0, 1}},
					{{1.5, 1, -1, 0}, { 1, 1 }, { 0, 1, 0, 1}},
					{{1.5, 0, -1, 0}, { 1, 0 }, { 0, 0, 1, 1}}
				})),
				new Material()
			);

		Entities::create()
			->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
			->setComponent<Visual>(
				new ProceduralMesh(new Vertexbuffer({
					{{0, 0, 1, 0}, { 0, 0 }, { 1, 0, 0, 1}},
					{{0, 0, 0, 0}, { 1, 1 }, { 0, 1, 0, 1}},
					{{1, 0, 0, 0}, { 1, 0 }, { 0, 0, 1, 1}}
				})),
				new Material()
			);

		Entities::create()
			->setComponent<Transform>(Vector3::zero, Quaternion::identity, Vector3::one)
			->setComponent<Visual>(
				new ProceduralMesh(new Vertexbuffer({
					{{0, 0, 1, 0}, { 0, 0 }, { 1, 0, 0, 1}},
					{{1, 0, 0, 0}, { 1, 1 }, { 0, 1, 0, 1}},
					{{1, 0, 1, 0}, { 1, 0 }, { 0, 0, 1, 1}}
				})),
				new Material()
			);

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

			const auto cameraSpeed = 1.f;
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
					cameraTransform->rotation(Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f));
					cameraForward = Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f) * Vector3::forward;
					cameraRight = Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f) * Vector3::right;					
					break;
				case GLFW_KEY_V:
					cameraAngle += deltaTime * 90.f;
					cameraTransform->rotation(Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f));
					cameraForward = Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f) * Vector3::forward;
					cameraRight = Quaternion::fromEulerAngles(0.f, zmath::radians(cameraAngle), 0.f) * Vector3::right;
					break;
				}
			}
		};

		e.mainLoop();
	}

	return 0;
}
