#pragma once
#include "app.h"


namespace one {
	class Pipeline {

	public:
		Pipeline(App& app);
		~Pipeline();
		void createPipeline();
		void createRenderPass();

		VkPipelineLayout pipelineLayout;

	private:

		App& _app;
		VkDevice& logicalDevice = _app.logicalDevice;
		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

	};
}