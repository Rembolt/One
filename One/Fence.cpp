#include "Fence.h"

namespace one {
	Fence::Fence(VkDevice _device) : _device(_device) {
		initialize();
	}

	void Fence::initialize() {
		//fences checks if gpu has finished something
		//we attach fences to work we submit to execute
		//unsignaled and signaled / 
		//first we set which operations set out the fence / then which part of the program is waiting on fence to signal
		//must be restet manually

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;//since first frame ca't wait on previous, fence must start signaled;

		if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphore objects!");
		}

		std::cerr << "vulkan fence has initiated \n";
	}

	bool Fence::waitForFence(uint64_t timeout)
	{
		return vkWaitForFences(_device, 1, &fence, VK_TRUE, timeout) == VK_SUCCESS;
	}

	/*bool Fence::waitForAllFences(uint64_t timeout)
	{
		return vkWaitForFences(_device, static_cast<uint32_t>(_fences.size()), _fences.data(), VK_TRUE, timeout) == VK_SUCCESS;
	}*/

	/*bool Fence::waitForAnyFences(uint64_t timeout)
	{
		return vkWaitForFences(_device, static_cast<uint32_t>(_fences.size()), _fences.data(), VK_FALSE, timeout) == VK_SUCCESS;
	}*/

	bool Fence::resetFence(void)
	{
		return vkResetFences(_device, 1, &fence) == VK_SUCCESS;
	}

	void Fence::destroy() {

	}

	Fence::~Fence() {

	}
}