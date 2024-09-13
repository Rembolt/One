#include "app.h"

namespace one {

	App::App() {
		initApp();
	}

	void App::initApp() {
		createInstance();
		pickPhysicalGraphicsDevice();

		std::cerr << "vulkan app has initiated \n";
	}

	void App::createInstance() {
		
		VkApplicationInfo appInfo{};
		VkInstanceCreateInfo createInfo{};

		//check if validation layers are in place:
		if (enableValidationLayers && !checkValidationLayerSupport()) {
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
		if (enableValidationLayers) {
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
	}

	bool App::checkValidationLayerSupport() {//check if validation layers are available
		
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

	void App::pickPhysicalGraphicsDevice() {

		physicalGraphicsDevice = VK_NULL_HANDLE;

		uint32_t deviceCount = 0;
		std::vector<VkPhysicalDevice> graphicDevices;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}else {
			graphicDevices.resize(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, graphicDevices.data());
		}

		//We can use multiple devices simultaneously or even 
		//choose the best device on machine. 
		//Now we are just getting highest score that also works:
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : graphicDevices) {
			int score = rateGraphicsDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}
		//Check if the best candidate is suitable at all


		for (auto rIt = candidates.rbegin(); rIt != candidates.crend(); rIt++)
		{
			if (rIt->first > 0) {
				physicalGraphicsDevice = rIt->second;
			}
		}
		if (physicalGraphicsDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	int App::rateGraphicsDeviceSuitability(VkPhysicalDevice graphicsDevice) {
		//As I add vulkan features I must comeback here 
		//to check if the gpu support those features
		VkPhysicalDeviceProperties graphicsDeviceProperties;//name, type, supported vulkan version
		vkGetPhysicalDeviceProperties(graphicsDevice, &graphicsDeviceProperties);
		VkPhysicalDeviceFeatures graphicsDeviceFeatures;//features like texture compression,64 bit floats,multi viewport rendering for VR
		vkGetPhysicalDeviceFeatures(graphicsDevice, &graphicsDeviceFeatures);

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (graphicsDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += graphicsDeviceProperties.limits.maxImageDimension2D;

		// Exemple Application can't function without geometry shaders
		/*if (!graphicsDeviceFeatures.geometryShader) {
			return 0;
		}*/

		std::cerr << "the device: "<< graphicsDeviceProperties.deviceID << " from vendor: " << graphicsDeviceProperties.vendorID << " had a score of: " << score << "\n";

		return score;
		//ended here
	}

	App::~App() {

		vkDestroyInstance(instance, nullptr);//pointer to callback

	}
}