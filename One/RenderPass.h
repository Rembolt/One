#pragma once
#include "UtilHeader.h"

namespace one {
	class RenderPass : NonCopyable
	{
	public:

		RenderPass(VkDevice _device, VkFormat _swapchainImageFormat);
		~RenderPass();

		void initialize(VkFormat _swapchainImageFormat);
		void destroy();

		inline VkRenderPass getRenderPass(void) const {
			return renderPass;
		}
	private:

		//handle device
		VkDevice _device;

		//
		VkRenderPass renderPass;

	};
}

