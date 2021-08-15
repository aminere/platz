
#include <GLFW/glfw3.h>

int main(void) {
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	const auto monitor = glfwGetPrimaryMonitor();
	const auto mode = glfwGetVideoMode(monitor);

	window = glfwCreateWindow(mode->width, mode->height, "Platz", monitor, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		// TODO render
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
