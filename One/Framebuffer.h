#pragma once
#include "vulkan/vulkan.h"
#include <vector>

namespace one {
	class Framebuffer
	{
	public:
		Framebuffer(VkDevice _device, const VkImageView attachments[], VkRenderPass renderPass, VkExtent2D extent);
		~Framebuffer();
		
		void destroy();

		inline VkFramebuffer getFrameBuffer() const{ 
			return frameBuffer;
		};

	private:
		
		void initialize(const VkImageView attachments[], VkRenderPass renderPass, VkExtent2D swapChainExtent);

		VkDevice _device;

		VkFramebuffer frameBuffer{VK_NULL_HANDLE};
		
		

	};
}
