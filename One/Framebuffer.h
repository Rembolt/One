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

		inline VkFramebuffer getFramebufferHandle() const{ 
			return framebufferHandle;
		};

		

	private:
		
		void initialize(const VkImageView attachments[], VkRenderPass renderPass, VkExtent2D& extent);

		VkDevice _logicalDevice;

		VkFramebuffer framebufferHandle{VK_NULL_HANDLE};
		
		

	};
}
