#pragma once

#include "vulkan/vulkan.h"

namespace one {
	class CommandPool
	{

	public:

		CommandPool(VkDevice logicalDevice, uint32_t queueIndex);
		~CommandPool();

		void initialize(uint32_t queueIndex);
		void destroy();

		inline VkCommandPool getCommandPoolHandler() const {
			return commandPool;
		}

	private:

		VkDevice _logicalDevice;

		VkCommandPool commandPool;
	};
}


