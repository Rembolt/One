#include "Framebuffer.h"
#include <stdexcept>
#include <iostream>

namespace one {

	Framebuffer::Framebuffer(VkDevice logicalDevice, const VkImageView attachments[], VkRenderPass renderPass, VkExtent2D& swapChainExtent) : _logicalDevice(logicalDevice) {
		initialize(attachments, renderPass, swapChainExtent);
	}
	
	void Framebuffer::initialize(const VkImageView attachments[], VkRenderPass renderPass, VkExtent2D& swapChainExtent) {
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;//must be compatible(use same attachments etc.)
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &framebufferHandle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create a framebuffer!");
		}
	}

	void Framebuffer::destroy() {
		if (framebufferHandle != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(_logicalDevice, framebufferHandle, nullptr);
			framebufferHandle = VK_NULL_HANDLE;
		}
	}

	Framebuffer::~Framebuffer() {
		destroy();
	}


}
