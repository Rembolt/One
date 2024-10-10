#pragma once
#include "UtilHeader.h"
#include <optional>
#include "Queue.h"
#include "Window.h"
#include "Swapchain.h"

namespace one {
	class Device : NonCopyable
	{
	public:

		Device(VkInstance _instance, const std::vector<const char*> validationLayers, Window* _window, SwapChain* pSwapChain);
		~Device();

		void initialize();
		void destroy();

		inline VkDevice getDevice(void) const {
			return device;
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

		VkDevice device;

		//Picking Graphics card device
		VkPhysicalDevice physicalGraphicsDevice;

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

	};
}

