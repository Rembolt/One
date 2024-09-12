#pragma once

#include "window.h"
#include "app.h"

namespace one {
	class One {
	public:
		const uint32_t WIDTH = 600;
		const uint32_t HEIGHT = 600;

		void run();

	private:
		Window window{ WIDTH, HEIGHT, "One" };
		App app;

		void initOne();
		void loop();
		void end();
	};
}
