#pragma once

#include "UtilHeader.h"
#include "Window.h"
#include "App.h"

namespace one {

	class One {
	public:
		const uint32_t WIDTH = 600;
		const uint32_t HEIGHT = 600;

		void run();

	private:
		Window* pWindow;
		App* pApp;

		void initOne();
		void loop();
		void end();
	};
}
