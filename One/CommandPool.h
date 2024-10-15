#pragma once
#include "UtilHeader.h"
#include "CommandBuffer.h"

namespace one {
	class CommandPool
	{

	public:

		CommandPool(VkDevice _device, uint32_t queueIndex);
		~CommandPool();

		void initialize(uint32_t queueIndex);
		void destroy();

		inline VkCommandPool getCommandPool() const {
			return commandPool;
		}

	private:

		VkDevice _device;

		VkCommandPool commandPool;

		CommandBuffer* commandBuffer;
	};
}


