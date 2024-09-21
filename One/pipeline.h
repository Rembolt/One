#pragma once
#include "app.h"


namespace one {
	class Pipeline {

	public:
		Pipeline(App& app);
		~Pipeline();
		void createPipeline();

	private:

		App& m_app;
		VkDevice& logicalDevice = m_app.logicalDevice;
		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);

	};
}