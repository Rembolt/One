#pragma once
#include "vulkan/vulkan.h"
#include <vector>

namespace one {
	class Pipeline {

	public:
		Pipeline(const Pipeline&) = delete;//cant pass by reference
		Pipeline& operator=(const Pipeline&) = delete;//cant copy by reference;
		
		Pipeline(VkDevice logicalDevice);
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
		
	private:
		
		VkDevice _logicalDevice;

		VkPipeline graphicsPipeline;
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;

		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

	};
}