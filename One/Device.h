#pragma once
#include "UtilHeader.h"
#include <optional>
#include "Queue.h"
#include "SwapChain.h"
#include "Window.h"


namespace one {
	class Device : NonCopyable
	{
	public:

		Device(VkInstance _instance, const std::vector<const char*> validationLayers,
			SwapChain* pSwapChain, Queue* pGraphicsQueue, Queue* pPresentationQueue);
		~Device();

		void initialize();
		void destroy();

		inline VkDevice getDevice() const {
			return device;
		}

		inline VkPhysicalDevice getPhysicalGraphicsDevice() const {
			return  physicalGraphicsDevice;
		}

	private:
		//handle of intance
		VkInstance _instance;

		//handle of window
		Window* _window;

		//Swapchain wrapper (handles images from vulkan to surface)
		SwapChain* pSwapChain;

		void pickPhysicalGraphicsDevice();
		int rateGraphicsDeviceSuitability(VkPhysicalDevice device);
		void initializeLogicalDevice();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);


		//Queue families
		bool findQueueFamilies(const VkPhysicalDevice graphicsDevice, Queue* pGraphicsQueue, Queue* pPrasentationQueue);

		//DEVICE handle
		VkDevice device;

		//Picking Graphics card device
		VkPhysicalDevice physicalGraphicsDevice = VK_NULL_HANDLE;

		//Device Extensions:
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		//Validation layers copy
		const std::vector<const char*> validationLayers;

		//Queues
		Queue* pGraphicsQueue;
		Queue* pPresentationQueue;

		

	};
}

