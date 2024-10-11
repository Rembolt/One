#pragma once
#include "UtilHeader.h"
#include "Window.h"
#include "ImageView.h"

namespace one {
	class SwapChain : NonCopyable
	{
	public:

		SwapChain(Window* _window, VkInstance _instance);
		~SwapChain();

		void initialize(VkDevice _device, VkPhysicalDevice physicalGraphicsDevice);
		void destroy();

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;// images on swap chain info, width and height of images etc
			std::vector<VkSurfaceFormatKHR> formats; // pixel format and color space
			std::vector<VkPresentModeKHR> presentationModes;
		};
		SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice graphicsDevice);

		uint32_t nextImage(VkDevice _device, VkSemaphore _semaphore);

		inline VkSurfaceKHR getSurface() const {
			return  surface;
		}

		inline VkFormat getImageFormat() const {
			return swapChainImageFormat;
		}

		inline VkExtent2D getExtent() const {
			return  swapChainExtent;
		}

		inline size_t getSwapChainImagesSize() const {
			return swapChainImages.size();
		}

		inline VkImageView getImageViews(int index) const {
			return pSwapChainImageViews[index]->getImageView();
		}

		inline VkSwapchainKHR getSwapChain() const {
			return swapChain;
		}


	private:
		void initializeImageViews();

		//swapchain(handles images from vulkan to surface)
		VkSwapchainKHR swapChain;

		VkDevice _device;

		//window to send surface to
		Window* pWindow;

		//surface to display image
		VkSurfaceKHR surface;

		//Image views(kind of like perspectives of image), depth, volumetric(?), etc.
		std::vector<ImageView*> pSwapChainImageViews;

		//SwapChain details
		std::vector<VkImage> swapChainImages;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	};
}

