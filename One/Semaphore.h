#pragma once
#include "UtilHeader.h"

namespace one {
	class Semaphore : NonCopyable
	{
	public:

		Semaphore(VkDevice _device);
		~Semaphore();

		void initialize();
		void destroy();

		inline VkSemaphore getSemaphore(void) const {
			return semaphore;
		}

	private:

		VkDevice _device;

		VkSemaphore semaphore;

	};
}