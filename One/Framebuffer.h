#pragma once
#include "vulkan/vulkan.h"
#include <vector>

namespace one {
	class Framebuffer
	{
	public:
		Framebuffer(VkDevice logicalDevice, const VkImageView attachments[], VkRenderPass renderPass, VkExtent2D& extent);
		~Framebuffer();
		
		void destroy();

		inline VkFramebuffer getFramebuffer() const{ 
			return frameBuffer;
		};

		

	private:
		
		void initialize(const VkImageView attachments[], VkRenderPass renderPass, VkExtent2D& extent);

		VkDevice _logicalDevice;

		VkFramebuffer frameBuffer{VK_NULL_HANDLE};
		
		

	};
}
