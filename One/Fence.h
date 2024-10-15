#pragma once
#include "UtilHeader.h"

namespace one {
	class Fence : NonCopyable
	{
	public:

		Fence(VkDevice _device);
		~Fence();

		void initialize();
		void destroy();

		inline VkFence getFence(void) const {
			return fence;
		}

		bool waitForFence(uint64_t timeout);
		bool resetFence(void);

	private:

		VkDevice _device;

		VkFence fence;

	};
}

