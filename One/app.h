#pragma once

#include "UtilHeader.h"
#include "Window.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "CommandBuffer.h"
#include "Device.h"
#include "Instance.h"
#include "RenderPass.h"
#include "Semaphore.h"
#include "Fence.h"


namespace one {

	class App {

	public:

		App(Window* pWindow);
		void initialize();
		void destroy();
		~App();
		

		App(const App&) = delete;//cant pass by reference
		App& operator=(const App&) = delete;//cant copy by reference;

		
		//action methods
		void drawFrame();

		//this is a manager/helper class it can't pass getters and setters to its objects but rather to its owners
		inline VkDevice getDevice(void) const {
			return _device;
		}

	private:

		
		void initializeFrameBuffers();
		void initializeCommandBuffer();
		void initializeSyncObjects();

		Instance* pInstance;
		Device* pDevice;
		//Swapchain wrapper (handles images from vulkan to surface)
		SwapChain* pSwapChain;
		//pipeline ptr
		Pipeline* pPipeline;
		RenderPass* pRenderPass;
		//FrameBuffers(linked to eache image, where data will be written to)
		std::vector<Framebuffer*> pSwapChainFramebuffers;
		CommandBuffer* pCommandBuffer;
		//Sync objects
		Semaphore* pImageAvailableSemaphore;
		Semaphore* pRenderFinishedSemaphore;
		Fence* pInFlightFence;
			
		//list of queues
		Queue* pGraphicsQueue;
		Queue* pPresentationQueue;


		//Window pointer
		Window* pWindow;

		//logical device
		VkDevice _device;
		
	};
}