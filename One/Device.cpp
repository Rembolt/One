#include "Device.h"
#include <map>
#include <set>


namespace one {
	Device::Device(VkInstance _instance, const std::vector<const char*> validationLayers, 
					SwapChain* pSwapChain, Queue* pGraphicsQueue, Queue* pPresentationQueue): 
					_instance(_instance), pSwapChain(pSwapChain), pPresentationQueue(pPresentationQueue), pGraphicsQueue(pGraphicsQueue) {
		initialize();
	}

	void Device::initialize() {
		pickPhysicalGraphicsDevice();
		initializeLogicalDevice();
	}

	void Device::pickPhysicalGraphicsDevice() {

		physicalGraphicsDevice = VK_NULL_HANDLE;

		uint32_t deviceCount = 0;
		std::vector<VkPhysicalDevice> graphicDevices;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		else {
			graphicDevices.resize(deviceCount);
			vkEnumeratePhysicalDevices(_instance, &deviceCount, graphicDevices.data());
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

	int Device::rateGraphicsDeviceSuitability(const VkPhysicalDevice graphicsDevice) {
		//As I add vulkan features I must comeback here 
		//to check if the gpu support those features
		VkPhysicalDeviceProperties graphicsDeviceProperties;//name, type, supported vulkan version
		vkGetPhysicalDeviceProperties(graphicsDevice, &graphicsDeviceProperties);
		VkPhysicalDeviceFeatures graphicsDeviceFeatures;//features like texture compression,64 bit floats,multi viewport rendering for VR
		vkGetPhysicalDeviceFeatures(graphicsDevice, &graphicsDeviceFeatures);

		int score = 0;

		// Exemple Application can't function without geometry shaders
		/*if (!graphicsDeviceFeatures.geometryShader) {
			return 0;
		}*/

		if (!findQueueFamilies(graphicsDevice, pGraphicsQueue, pPresentationQueue)) {
			std::cerr << "this graphics device " << graphicsDeviceProperties.deviceID << " does not support the queues neccessary for the vulkan commands used!\n";
			return 0;
		}
		if (!checkDeviceExtensionSupport(graphicsDevice)) {
			std::cerr << "this graphics device " << graphicsDeviceProperties.deviceID << " does not support all extensions necessary to run program!\n";
			return 0;
		}
		SwapChain::SwapChainSupportDetails swapChainSupport = pSwapChain->querySwapChainSupport(graphicsDevice);
		//for now our swapchain just needs 1 image format, 1 presentation mode
		if (swapChainSupport.formats.empty() || swapChainSupport.presentationModes.empty()) {
			std::cerr << "this graphics device and surface dont hold enough compatible image formats and presentation modes!\n";
			return 0;
		}

		// Discrete GPUs have a significant performance advantage
		score += 1000 * (graphicsDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		//if both are same performance is faster
		score += 1000 * (pGraphicsQueue->getFamilyIndex() == pPresentationQueue->getFamilyIndex());
		// Maximum possible size of textures affects graphics quality
		score += graphicsDeviceProperties.limits.maxImageDimension2D;

		std::cerr << "the device: " << graphicsDeviceProperties.deviceID << " from vendor: " << graphicsDeviceProperties.vendorID << " had a score of: " << score << "\n";

		return score;
	}

	void Device::initializeLogicalDevice() {
		assert(findQueueFamilies(physicalGraphicsDevice, pGraphicsQueue, pPresentationQueue));

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t>  uniqueQueueFamilies = {
			pGraphicsQueue->getFamilyIndex(),
			pPresentationQueue->getFamilyIndex()
		};

		//most times both are the same
		float queuePriority = 1.0f;//priority of which queue to prioritize
		//right now just creating one, fix findQueues function if 2 different needed
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};//none for now

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		if (ENABLE_VALIDATION_LAYER) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physicalGraphicsDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		std::cerr << "vulkan device has initiated \n";
	}

	bool Device::checkDeviceExtensionSupport(const VkPhysicalDevice graphicsDevice) {

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(graphicsDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(graphicsDevice, nullptr, &extensionCount, availableExtensions.data());

		for (const char* extensionName : deviceExtensions) {
			bool extensionFound = false;

			for (const auto& extension : availableExtensions) {
				if (strcmp(extensionName, extension.extensionName) == 0) {
					extensionFound = true;
					break;
				}
			}

			if (!extensionFound) {
				return false;
			}
		}
		return true;
	}

	bool Device::findQueueFamilies(const VkPhysicalDevice graphicsDevice, Queue* pGraphicsQueue, Queue* pPrasentationQueue) {

		//on vulkan any command sent to vulkan is submitted to a queue
		//there is different types of queues with different sizes
		//this checks(for now) for any queues that support graphics

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(graphicsDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(graphicsDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		int queueSet = 0;
		//marks the flag indice of the 
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				//queueFamily that has atleast the graphics bit
				assert(pGraphicsQueue->setFamilyIndex(i));
				assert(pGraphicsQueue->setQueueCount(queueFamily.queueCount));
				queueSet++;
			}
			//if presentationsupport is available on this queue being checked
			VkBool32 presentationSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(graphicsDevice, i, pSwapChain->getSurface(), &presentationSupport);
			if (presentationSupport) {
				assert(pPresentationQueue->setFamilyIndex(i));
				assert(pPresentationQueue->setQueueCount(queueFamily.queueCount));
				queueSet++;
			}
			if (queueSet >=2) {
				return true;
			}
			i++;
		}

		return false;
	}

	void Device::destroy() {
		if (device != VK_NULL_HANDLE) {
			vkDestroyDevice(device, nullptr);
			device = VK_NULL_HANDLE;
		}
		
	}
	Device::~Device() {
		destroy();
	}
}