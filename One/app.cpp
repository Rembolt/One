#include "app.h"
#include <map>
#include <set>
#include <algorithm> // Necessary for std::clamp

//Ctrl + M, then O to collapse all functions

namespace one {

	App::App(Window* pWindow): pWindow(pWindow) {
	}

	void App::initialize() {
		pInstance = new Instance();

		pSwapChain = new SwapChain(pWindow, pInstance->getInstance());

		pDevice = new Device(pInstance->getInstance(), pInstance->getInstance(), pWindow, pSwapChain);
		_device = pDevice->getDevice();

		//beacause only one queueFamily and only one queue on the family
		//we will onyly do this once for now
		Device::QueueFamilyIndices indices = pDevice->findQueueFamilies(physicalGraphicsDevice);
		pGraphicsQueue = new Queue(_device, indices.graphicsFamily.value());
		pPresentationQueue = new Queue(_device, indices.presentationFamily.value());

		pSwapChain->initialize();

		pSwapChain->initializeImageViews();

		pPipeline = new Pipeline(_device);

		pRenderPass = new RenderPass(_device, pSwapChain->getSwapChainImageFormat);

		initializeFrameBuffers();

		initializeCommandBuffer();

		//recordCommandBuffer();
		
		initializeSyncObjects();
		initializeSyncObjects();

		std::cerr << "vulkan app has initiated \n";
	}


	void App::initializeFrameBuffers() {
		int swapChainImageSize = pSwapChain->getSwapChainImageSize();
		pSwapChainFramebuffers.resize(swapChainImageSize);

		for (size_t i = 0; i < swapChainImageSize; i++) {
			VkImageView attachments[] = {
				pSwapChain->getImageViews(i);
			};
			//allocating to heap
			pSwapChainFramebuffers[i] = new Framebuffer(_device, attachments, pRenderPass->getRenderPass(), pSwapChain->getSwapChainImageSize());
		}
	}

	void App::initializeCommandBuffer() {
		pCommandBuffer = new CommandBuffer(_device, pDevice->getCommandPool());
	}

	void App::initializeSyncObjects(){
		pImageAvailableSemaphore = new Semaphore(_device);
		pRenderFinishedSemaphore new Semaphore(_device);
		pInFlightFence = new Fence(_device);
	}


	void App::drawFrame() {
		//rendering a frame consits of these steps:
		//wait for previous frame to finish
		//acquire an image from swapchain - gpu
		//record a command buffer which draws the scene onto that image 
		//submit the recorded command buffer(execute) - gpu
		//present the swap chain image - gpu

		//wait for previous frame draw sequence
		//array of fences waits for one or all, also has timeout but max int basically disables it
		assert(pInFlightFence->waitForFence(UINT64_MAX));
		//reset it
		assert(pInFlightFence->resetFence());

		//acquire image from swapchain - gpu
		//timeout to maxint to disable, it will sugnal the image available semaphore
		uint32_t imageIndex = pSwapChain->nextImage(_device, &pImageAvailableSemaphore->getSemaphore());

		//record a command buffer which draws the scene onto that image 
		//reset it to make sure can be drawn
		assert(vkResetCommandBuffer(pCommandBuffer, 0));
		//starts pipeline and renderpass aiming at framebuffer[imageIndex] and adds draw command to buffer
		pCommandBuffer->recordCommandBuffer(pSwapChainFramebuffers[i]->getFrameBuffer(), 
			pRenderPass->getRenderPass(), pPipeline->getPipeline(), pSwapChain->getExtent());

		//submit the recorded command buffer(execute) - gpu
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore waitSemaphores[] = { &pImageAvailableSemaphore->getSemaphore() };//which semaphore to wait on
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };//which stage of pipeline to wait on
		submitInfo.waitSemaphoreCount = 1;
		//each index of each array corresponds to each other
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;	
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		//which semaphores to signal when done
		VkSemaphore signalSemaphores[] = { &pRenderFinishedSemaphore->getSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		//ended here(add queue submit to queue object and fix queue structure thingy
		if (vkQueueSubmit(pGraphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer to queue!");
		}

		//present the swap chain image - gpu
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;//waiting render to finish

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;//image we drew framebuffer to(index sync)
		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(presentationQueue, &presentInfo);
	}

	App::~App() {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
		vkDestroyFence(logicalDevice, inFlightFence, nullptr);

		commandPool->destroy();
		delete commandPool;

		for (auto framebuffer : swapChainFramebuffers) {
			framebuffer->destroy(); 
			//must delete pointer after deleting object
			delete framebuffer;
		}
		swapChainFramebuffers.clear();

		pipeline->destroyPipeline();

		pipeline->destroyRenderPass();

		//destroy image views(created by us)
		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(logicalDevice, imageView, nullptr);
		}
		swapChainImageViews.clear();

		vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

		vkDestroyDevice(logicalDevice, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);

		vkDestroyInstance(instance, nullptr);//pointer to callback
	}
}