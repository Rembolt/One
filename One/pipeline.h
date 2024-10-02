#pragma once
#include "vulkan/vulkan.h"
#include <vector>

namespace one {
	class Pipeline {

	public:
		
		Pipeline();
		~Pipeline();

		//constructors
		void createPipeline();
		void createRenderPass(VkFormat _swapchainImageFormat);

		//destructors
		void destroyPipeline();
		void destroyRenderPass();

		//getters & setters
		inline VkPipeline getGraphicsPipeline(void) const {
			return graphicsPipeline;
		}

		inline VkRenderPass getRenderPass(void) const {
			return renderPass;
		}

		inline void setLogicalDevice(VkDevice logicalDevice) {
			_logicalDevice = logicalDevice;
		}
		
	private:
		
		VkDevice _logicalDevice;

		VkPipeline graphicsPipeline;
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;

		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

	};
}