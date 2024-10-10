#include "CommandPool.h"

namespace one {
	CommandPool::CommandPool(VkDevice _device, uint32_t queueIndex): _device(_device) {
		initialize(queueIndex);
	}

	void CommandPool::initialize(uint32_t queueIndex) {
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		//two types of flags: Transient= newcommands veryoften / reset= commands will be recorded individually(allows to reuse without destroying them)
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueIndex;
		//command pool can only contain commands to a single queue

		if (vkCreateCommandPool(_device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}

	}

	void CommandPool::destroy() {
		if (commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(_device, commandPool, nullptr);
			commandPool = VK_NULL_HANDLE;
		}
	}

	CommandPool::~CommandPool() {
		destroy();
	}
}
