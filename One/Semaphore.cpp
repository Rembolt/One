#include "Semaphore.h"

namespace one {
	Semaphore::Semaphore(VkDevice _device) : _device(_device) {
		initialize();
	}

	void Semaphore::initialize() {
		//semaphore adds order to queue operations(commands) on gpu
		//in same queue and different queues / there are 2 types binary and timeline
		//for now only using binary
		//unsignaled and signaled
		//first we set which operations set out the semaphore / then which operations are waiting on semaphore to signal
		//resets automatically

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS ){
			throw std::runtime_error("failed to create semaphore objects!");
		}
	}

	void Semaphore::destroy() {

	}
	Semaphore::~Semaphore() {

	}
}