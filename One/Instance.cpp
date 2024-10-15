#include "Instance.h"

namespace one {
	Instance::Instance() {
		initialize();
	}

	void Instance::initialize() {
		VkApplicationInfo appInfo{};
		VkInstanceCreateInfo createInfo{};

		//check if validation layers are in place:
		if (ENABLE_VALIDATION_LAYER && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "One";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		//tells the Vulkan driver which global extensions to use

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; //most structs in vulkan require to specify their type
		createInfo.pApplicationInfo = &appInfo;

		//Using GLFW might move to window in future;
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);;

		std::vector<const char*> requiredExtensions;
		//when time come be to add extra extensions:(Can check if extensions are supported by vulkan)
		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			requiredExtensions.emplace_back(glfwExtensions[i]);
		}

		createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		//tells Vulkan driver which validation layers to use
		if (ENABLE_VALIDATION_LAYER) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}


		//middle is a pointer to custom allocator callbacks
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vulkan instance!");
		}

		//MAC stuff:
		/*requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;*/

		std::cerr << "vulkan instance has initiated \n";
	}


	bool Instance::checkValidationLayerSupport() {//check if validation layers are available

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}
		return true;
	}

	void Instance::destroy() {

	}
	Instance::~Instance() {

	}
}