#pragma once

#include "UtilHeader.h"
#include <string>

namespace one {

	class Window {
	public:
		Window(uint32_t w, uint32_t h, std::string name);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		
		bool shouldClose();
		void createSurface(const VkInstance& instance, VkSurfaceKHR& surface);
		void getFramebufferSize(int& width, int& height);

	private:
		void initWindow();

		const uint32_t WIDTH;
		const uint32_t HEIGHT;
		std::string windowName;

		GLFWwindow* window;
	};
}
