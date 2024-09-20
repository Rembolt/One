#include "app.h"
#include <map>
#include <set>
#include <algorithm> // Necessary for std::clamp

//Ctrl + M, then O to collapse all functions

namespace one {

	App::App(Window& window): m_window(window) {
		
	}

	void App::initApp() {
		createInstance();
		m_window.createSurface(instance,surface);
		pickPhysicalGraphicsDevice();
		createLogicalDevice();
		//accessed publicly
		createSwapChain();
		createImageViews();


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

	int App::rateGraphicsDeviceSuitability(const VkPhysicalDevice graphicsDevice) {
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
		QueueFamilyIndices indices = findQueueFamilies(graphicsDevice);
		if (!indices.isComplete()) {
			std::cerr << "this graphics device " << graphicsDeviceProperties.deviceID << " does not support the queues neccessary for the vulkan commands used!\n";
			return 0;
		}
		if (!checkDeviceExtensionSupport(graphicsDevice)) {
			std::cerr << "this graphics device " << graphicsDeviceProperties.deviceID << " does not support all extensions necessary to run program!\n";
			return 0;
		}
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(graphicsDevice);
		//for now our swapchain just needs 1 image format, 1 presentation mode
		if (swapChainSupport.formats.empty() || swapChainSupport.presentationModes.empty()) {
			std::cerr << "this graphics device and surface dont hold enough compatible image formats and presentation modes!\n";
			return 0;
		}
		
		// Discrete GPUs have a significant performance advantage
		score += 1000 * (graphicsDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		//if both are same performance is faster
		score += 1000 * (indices.graphicsFamily == indices.presentationFamily);
		// Maximum possible size of textures affects graphics quality
		score += graphicsDeviceProperties.limits.maxImageDimension2D;

		std::cerr << "the device: "<< graphicsDeviceProperties.deviceID << " from vendor: " << graphicsDeviceProperties.vendorID << " had a score of: " << score << "\n";

		return score;
	}

	App::QueueFamilyIndices App::findQueueFamilies(const VkPhysicalDevice graphicsDevice) {
		QueueFamilyIndices indices;
		//on vulkan any command sent to vulkan is submitted to a queue
		//there is different types of queues with different sizes
		//this checks(for now) for any queues that support graphics

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(graphicsDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(graphicsDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		//marks the flag indice of the 
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				//queueFamily that has atleast the graphics bit
				indices.graphicsFamily = i;
			}
			//if presentationsupport is available on this queue being checked
			VkBool32 presentationSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(graphicsDevice, i, surface, &presentationSupport);
			if (presentationSupport) {
				indices.presentationFamily = i;
			}
			if (indices.isComplete()) {
				break;
			}
			i++;
		}

		return indices;
	}

	void App::createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalGraphicsDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			indices.graphicsFamily.value(),
			indices.presentationFamily.value()
		};

		float queuePriority = 1.0f;//priority of which queue to prioritize
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queuePriority -= 0.01f;//decreases priority as you go down the list
			//THIS MIGHT CAUSE PROBLEMS
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
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physicalGraphicsDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		//beacause only one queueFamily and only one queue on the family
		//we will onyly do this once for now
		vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(logicalDevice, indices.presentationFamily.value(), 0, &presentationQueue);
	}

	bool App::checkDeviceExtensionSupport(const VkPhysicalDevice graphicsDevice) {

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

	App::SwapChainSupportDetails App::querySwapChainSupport(const VkPhysicalDevice graphicsDevice) {
		
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphicsDevice, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(graphicsDevice, surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(graphicsDevice, surface, &formatCount, details.formats.data());
		}

		uint32_t modeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(graphicsDevice, surface, &modeCount, nullptr);
		if (modeCount != 0) {
			details.presentationModes.resize(modeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(graphicsDevice, surface, &modeCount, details.presentationModes.data());
		}
		
		return details;
	}

	//color depth format preference
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

		//format is the format of each color / colorspace shows if srgb is supported or not
		for (const auto& surfaceFormat : availableFormats) {
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return surfaceFormat;
			}
		}
		return availableFormats[0];
	}

	//prefered conditions filter to show image to screen(modes)
	// immediate-no filter(may cause tearing)
	// fifo-waits if image queue is full
	// fifoRelaxed-same but if queue empty it sends it right away(may cause tearing)
	// mailbox-if queue is full it replaces lasst images with the new ones(less latency)
	VkPresentModeKHR chooseSwapPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes) {
		
		for (const auto& presentationMode : availablePresentationModes) {
			if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return presentationMode;
			}
		}
		
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	//swapextent is the resolution of swapchain
	//most times it is equal to the window resolution
	//sometimes resolution is different from pixels so we must get pixels from frameBuffer
	VkExtent2D App::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

		//if it is max it means currentExtent.width isn't set, meaning window is in window mode and must be manually determined.
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}else {
			int width, height;
			m_window.getFramebufferSize(width,height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};
			//clamp bounds the first value to the closest number inside a min and a max if not already
			actualExtent.width = std::clamp(
				actualExtent.width,
				capabilities.minImageExtent.width,//surface supported min and max width and height
				capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(
				actualExtent.height,
				capabilities.minImageExtent.height,
				capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	void App::createSwapChain() {
		//get supported by graphics device and surface
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalGraphicsDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentationMode = chooseSwapPresentationMode(swapChainSupport.presentationModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;// image in the swapChain may have to add more futurely
		if (swapChainSupport.capabilities.maxImageCount > 0 && 
			imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;// not one if stereoscopic 3D application(Vr 2 eye perspective)
		//images usage that adds color to images in swap chain and sends to surrface to be displayed on window
		//when adding postprocessing I can change it to VK_IMAGE_USAGE_TRANSFER_DST_BIT
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//if the graphics queue family and the presentation one are different 
		//we must do ownership chapters to specify
		//which queue family owns each image
		QueueFamilyIndices indices = findQueueFamilies(physicalGraphicsDevice);
		//uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentationFamily.value() };
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;

		//you could apply transforms to images in swapchain, but we wont need that
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		//how to use alpha info in the pixels going to the window, if they are 
		//consider we can use transparency with outher windows
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentationMode;
		//what happens to pixes that get covered by other windows,if true they are not considered
		createInfo.clipped = VK_TRUE;
		//if swapchain becames obsolete(like we changed the window size)
		//we must create a new one and the old one needs to be referenced here;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap-chain!");
		}

		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void App::createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			//could be 1D textures, 2D textures, 3D textures and cube maps.
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;

			//can map the color channels differently( exp. monochrome = allpoitn to one))
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;	
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			//
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;//purpose
			createInfo.subresourceRange.baseMipLevel = 0;//mipmapping levels
			createInfo.subresourceRange.levelCount = 1;//?
			createInfo.subresourceRange.baseArrayLayer = 0;//purpose
			createInfo.subresourceRange.layerCount = 1;//multiple layers per view

			if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image views!");
			}
		}
	}

	App::~App() {
		//destroy image views(created by us)
		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(logicalDevice, imageView, nullptr);
		}

		vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
		vkDestroyDevice(logicalDevice, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);//pointer to callback
	}
}