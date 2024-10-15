#pragma once
#include "UtilHeader.h"

namespace one {
	class Instance : NonCopyable
	{
	public:

		Instance();
		~Instance();

		void initialize();
		bool checkValidationLayerSupport();
		void destroy();

		inline VkInstance getInstance(void) const {
			return instance;
		}

		inline const std::vector<const char*> getValidationLayers(void) const {
			return validationLayers;
		}

	private:

		


		//Creating Vulkan Instance
		VkInstance instance;

		//Validation layers:
		const std::vector<const char*> validationLayers = {//if I ever need to debug other platforms might need to add other validations
			"VK_LAYER_KHRONOS_validation"
		};

	};
}

