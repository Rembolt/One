#pragma once
#include "app.h"

namespace one {
	class Pipeline {

	public:
		App& _app;
		Pipeline(App& app);
		~Pipeline();

		//constructors
		void createPipeline();
		void createRenderPass();

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
		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);
	
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;

	};
}