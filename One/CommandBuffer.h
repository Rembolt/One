#pragma once
#include "vulkan/vulkan.h"

namespace one {
	class CommandBuffer
	{
	public:

		CommandBuffer();
		~CommandBuffer();

		void initialize();
		void destroy();


	private:

		VkCommandBuffer commandBuffer;

	};
}


