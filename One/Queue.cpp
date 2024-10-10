#include "Queue.h"

namespace one {
	Queue::Queue(VkDevice _device, uint32_t familyIndex) : _device(_device), familyIndex(familyIndex){
		assert(initialize());
		initializeCommandPool();
	}

	bool Queue::initialize() {

		vkGetDeviceQueue(_device, familyIndex, 0, &queue);
		return true;
	}

	void Queue::initializeCommandPool() {

		commandPool = new CommandPool(_device, familyIndex);
	}

	void Queue::destroy() {

	}
	Queue::~Queue() {

	}
}