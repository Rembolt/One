#include "SwapChain.h"
#include <algorithm> // Necessary for std::clamp

namespace one {
	SwapChain::SwapChain( Window* pWindow, VkInstance _instance) : pWindow(pWindow) {
		pWindow->createSurface(_instance, surface);
	}


	void SwapChain::initialize(VkDevice _device, VkPhysicalDevice physicalGraphicsDevice /*uint32_t queueFamilyIndices[]*/) {
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

		if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap-chain!");
		}

		vkGetSwapchainImagesKHR(_device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(_device, swapChain, &imageCount, swapChainImages.data());
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;

		initializeImageViews(_device);

		std::cerr << "vulkan KHR swapchain has initiated \n";
	}

	SwapChain::SwapChainSupportDetails SwapChain::querySwapChainSupport(const VkPhysicalDevice graphicsDevice) {

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
	VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

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
	VkPresentModeKHR SwapChain::chooseSwapPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes) {

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
	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

		//if it is max it means currentExtent.width isn't set, meaning window is in window mode and must be manually determined.
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			pWindow->getFramebufferSize(width, height);

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

	void SwapChain::initializeImageViews(VkDevice _device) {
		pSwapChainImageViews.resize(swapChainImages.size());
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			pSwapChainImageViews[i] = new ImageView(_device, swapChainImages[i], swapChainImageFormat);
		}
	}

	uint32_t SwapChain::nextImage(VkDevice _device, VkSemaphore _semaphore) {
		uint32_t imageIndex;
		vkAcquireNextImageKHR(_device, swapChain, UINT64_MAX,_semaphore, VK_NULL_HANDLE, &imageIndex);
		return imageIndex;
	}

	void SwapChain::destroy() {

	}
	SwapChain::~SwapChain() {

	}
}