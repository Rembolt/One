#pragma once

#include "window.h"
#include <vector>
#include <optional>
#include "Framebuffer.h"
#include "pipeline.h"

namespace one {

	class App {

	public:

		App(Window& window);
		void initApp();
		~App();
		

		App(const App&) = delete;//cant pass by reference
		App& operator=(const App&) = delete;//cant copy by reference;

		//check if c++ is compiling in anything other than debug mode
		#ifdef NDEBUG
				const bool enableValidationLayers = false;
		#else
				const bool enableValidationLayers = true;
		#endif

		//this is a manager/helper class it can't pass getters and setters to its objects

	private:

		void createInstance();
		bool checkValidationLayerSupport();
		void pickPhysicalGraphicsDevice();
		int rateGraphicsDeviceSuitability(VkPhysicalDevice device);
		void createLogicalDevice();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		void createSwapChain();
		void createFrameBuffers();
		void createImageViews();


		//Creating Vulkan Instance
		VkInstance instance;

		//Picking Graphics card device
		VkPhysicalDevice physicalGraphicsDevice;

		//Queue families
		struct QueueFamilyIndices {
			//opitional adds has_value() to int
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentationFamily;//make sure queues can draw on surface


			bool isComplete() {
				//if flag supports graphics bit it will have the indice of it
				return graphicsFamily.has_value() && presentationFamily.has_value();
			}
		};
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice graphicsDevice);	

		//Creating Logical device
		VkDevice logicalDevice;
		VkQueue graphicsQueue;
		VkQueue presentationQueue;

		//Surface
		VkSurfaceKHR surface;

		//Window pointer
		Window& _window;

		//Validation layers:
		const std::vector<const char*> validationLayers = {//if I ever need to debug other platforms might need to add other validations
			"VK_LAYER_KHRONOS_validation"
		};

		//Device Extensions:
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		//SwapChain details
		std::vector<VkImage> swapChainImages;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;
		VkSwapchainKHR swapChain;
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;// images on swap chain info, width and height of images etc
			std::vector<VkSurfaceFormatKHR> formats; // pixel format and color space
			std::vector<VkPresentModeKHR> presentationModes;
		};
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice graphicsDevice);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		//Image views(kind of like perspectives of image), depth, volumetric(?), etc.
		std::vector<VkImageView> swapChainImageViews;

		//pipeline
		Pipeline pipeline;

		//FrameBuffers(linked to eache image, where data will be written to)
		std::vector<Framebuffer*> swapChainFramebuffers;
	};
}