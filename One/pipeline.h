#pragma once
#include "UtilHeader.h"

namespace one {
	class Pipeline : NonCopyable{

	public:
		Pipeline(const Pipeline&) = delete;//cant pass by reference
		Pipeline& operator=(const Pipeline&) = delete;//cant copy by reference;
		
		Pipeline(VkDevice _device, VkRenderPass _renderPass);
		~Pipeline();

		//constructors
		void initialize(VkRenderPass _renderPass);

		//destructors
		void destroy();

		//getters & setters
		inline VkPipeline getPipeline(void) const {
			return pipeline;
		}
		
	private:
		
		VkDevice _device;

		VkPipeline pipeline;

		VkPipelineLayout pipelineLayout;

		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

	};
}