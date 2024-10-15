#pragma once
#include "UtilHeader.h"

namespace one {
	class CommandBuffer : NonCopyable
	{
	public:

		CommandBuffer(VkDevice _device, VkCommandPool _commandPool);
		~CommandBuffer();

		void initialize();
		void destroy();
		
		void recordCommandBuffer(VkFramebuffer frameBuffer, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkExtent2D swapChainExtent);

		void reset();

		inline VkCommandBuffer getCommandBuffer() const {
			return commandBuffer;
		}

		inline const VkCommandBuffer* getCommandBufferPointer() const {
			return &commandBuffer;
		}
		

	private:

		VkDevice _device;
		
		VkCommandPool _commandPool;

		VkCommandBuffer commandBuffer;
		
	};
}


