#pragma once

#include "window.h"

namespace one {

	class App {

	public:

		App();
		~App();


	private:

		void initApp();
		void createInstance();

		VkInstance instance;

	};
}