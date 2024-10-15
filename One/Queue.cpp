#include "Queue.h"


namespace one {
	Queue::Queue(uint32_t familyIndex, float priority) :  familyIndex(familyIndex), priority(priority){

	}

	bool Queue::initialize(VkDevice _device) {
		Queue::_device = _device;
		vkGetDeviceQueue(_device, familyIndex, 0, &queue);
		initializeCommandPool();

		std::cerr << "vulkan queue has initiated \n";
		return true;
	}

	void Queue::submit(VkSubmitInfo submitInfo, Fence* fence) {
		//ended here(add queue submit to queue object and fix queue structure thingy
		if (vkQueueSubmit(queue, 1, &submitInfo, fence->getFence()) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer to queue!");
		}
	}

	void Queue::present(VkPresentInfoKHR presentInfo) {

		if (vkQueuePresentKHR(queue, &presentInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to present image from queue!");
		}
	}

	void Queue::initializeCommandPool() {

		pCommandPool = new CommandPool(_device, familyIndex);
	}

	void Queue::destroy() {

	}
	Queue::~Queue() {

	}
}