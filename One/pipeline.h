#pragma once
#include "UtilHeader.h"

namespace one {
	class Pipeline : NonCopyable{

	public:
		Pipeline(const Pipeline&) = delete;//cant pass by reference
		Pipeline& operator=(const Pipeline&) = delete;//cant copy by reference;
		
		Pipeline(VkDevice _device);
		~Pipeline();

		//constructors
		void createPipeline();

		//destructors
		void destroy();

		//getters & setters
		inline VkPipeline getGraphicsPipeline(void) const {
			return graphicsPipeline;
		}
		
	private:
		
		VkDevice _device;

		VkPipeline graphicsPipeline;

		VkPipelineLayout pipelineLayout;

		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

	};
}