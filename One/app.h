#pragma once

#include "window.h"
#include <vector>
#include <map>

namespace one {

	class App {

	public:

		App();
		~App();

		const std::vector<const char*> validationLayers = {//if I ever need to debug other platforms might need to add other validations
			"VK_LAYER_KHRONOS_validation"
		};

		//check if c++ is compiling in anything other than debug mode
		#ifdef NDEBUG
				const bool enableValidationLayers = false;
		#else
				const bool enableValidationLayers = true;
		#endif


	private:

		void initApp();
		void createInstance();
		bool checkValidationLayerSupport();
		void pickPhysicalGraphicsDevice();
		int rateGraphicsDeviceSuitability(VkPhysicalDevice device);

		
		//Creating Vulkan Instance
		VkInstance instance;

		//Picking Graphics card device
		VkPhysicalDevice physicalGraphicsDevice;

	};
}