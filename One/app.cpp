#include "app.h"
#include <map>
#include <set>
#include <algorithm> // Necessary for std::clamp

//Ctrl + M, then O to collapse all functions

namespace one {

	App::App(Window* pWindow): pWindow(pWindow) {
		initialize();
	}

	void App::initialize() {
		pInstance = new Instance();

		pSwapChain = new SwapChain(pWindow, pInstance->getInstance());

		pGraphicsQueue = new Queue(-1, 1.0f);
		pPresentationQueue = new Queue(-1, 1.0f);

		pDevice = new Device(pInstance->getInstance(), pInstance->getValidationLayers(), pSwapChain, pGraphicsQueue, pPresentationQueue);
		_device = pDevice->getDevice();

		assert(pGraphicsQueue->initialize(_device));
		assert(pPresentationQueue->initialize(_device));

		pSwapChain->initialize(_device, pDevice->getPhysicalGraphicsDevice());
		
		pRenderPass = new RenderPass(_device, pSwapChain->getImageFormat());

		pPipeline = new Pipeline(_device, pRenderPass->getRenderPass());

		initializeFrameBuffers();

		initializeCommandBuffer();

		//recordCommandBuffer();
		
		initializeSyncObjects();

		std::cerr << "app has initiated \n";
	}


	void App::initializeFrameBuffers() {
		int swapChainImageSize = pSwapChain->getSwapChainImagesSize();
		pSwapChainFramebuffers.resize(swapChainImageSize);

		for (size_t i = 0; i < swapChainImageSize; i++) {
			VkImageView attachments[] = {
				pSwapChain->getImageViews(i)
			};
			//allocating to heap
			pSwapChainFramebuffers[i] = new Framebuffer(_device, attachments, pRenderPass->getRenderPass(), pSwapChain->getExtent());
		}
	}

	void App::initializeCommandBuffer() {
		pCommandBuffer = new CommandBuffer(_device, pGraphicsQueue->getCommandPool());
	}

	void App::initializeSyncObjects(){
		pImageAvailableSemaphore = new Semaphore(_device);
		pRenderFinishedSemaphore = new Semaphore(_device);
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
		uint32_t imageIndex = pSwapChain->nextImage(_device, pImageAvailableSemaphore->getSemaphore());

		//record a command buffer which draws the scene onto that image 
		//reset it to make sure can be drawn
		pCommandBuffer->reset();
		//starts pipeline and renderpass aiming at framebuffer[imageIndex] and adds draw command to buffer
		pCommandBuffer->recordCommandBuffer(pSwapChainFramebuffers[imageIndex]->getFrameBuffer(), 
											pRenderPass->getRenderPass(), pPipeline->getPipeline(), 
											pSwapChain->getExtent());

		//submit the recorded command buffer(execute) - gpu
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore waitSemaphores[] = { pImageAvailableSemaphore->getSemaphore() };//which semaphore to wait on
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };//which stage of pipeline to wait on
		submitInfo.waitSemaphoreCount = 1;
		//each index of each array corresponds to each other
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;	
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = pCommandBuffer->getCommandBufferPointer();
		//which semaphores to signal when done
		VkSemaphore signalSemaphores[] = { pRenderFinishedSemaphore->getSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		//ended here(add queue submit to queue object and fix queue structure thingy
		pGraphicsQueue->submit(submitInfo, pInFlightFence);

		//present the swap chain image - gpu
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;//waiting render to finish

		VkSwapchainKHR swapChains[] = { pSwapChain->getSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;//image we drew framebuffer to(index sync)
		presentInfo.pResults = nullptr;

		pPresentationQueue->present(presentInfo);
		
	}

	void App::destroy() {
		pImageAvailableSemaphore->destroy();
		delete pImageAvailableSemaphore;
		pRenderFinishedSemaphore->destroy();
		delete pRenderFinishedSemaphore;
		pInFlightFence->destroy();
		delete pInFlightFence;

		pGraphicsQueue->destroy();

		for (auto framebuffer : pSwapChainFramebuffers) {
			framebuffer->destroy();
			//must delete pointer after deleting object
			delete framebuffer;
		}
		pSwapChainFramebuffers.clear();

		pPipeline->destroy();
		delete pPipeline;

		pRenderPass->destroy();
		delete pRenderPass;

		pSwapChain->destroy(_device);
		delete pSwapChain;

		//queue is only deleted close to device and can't be vkDestroyed
		delete pGraphicsQueue;
		delete pPresentationQueue;

		pDevice->destroy();
		delete pDevice;

		assert(pWindow->destroySurface(pInstance->getInstance(), pSwapChain->getSurface()));

		pInstance->destroy();
		delete pInstance;
	}

	App::~App() {
		
		
	}
}