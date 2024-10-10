#pragma once
#include "UtilHeader.h"
#include "CommandPool.h"

namespace one {
	class Queue : NonCopyable
	{
	public:

		Queue(VkDevice _device, uint32_t familyIndex);
		~Queue();

		bool initialize();
		void destroy();
		void initializeCommandPool();

		inline VkQueue getQueue(void) const{
			return queue;
		}

		inline uint32_t getFamilyIndex(void) const {
			return familyIndex;
		}

		inline VkCommandPool getCommandPool(void) const {
			return commandPool->getCommandPool();
		}


	private:

		VkDevice _device;

		VkQueue queue;
		uint32_t familyIndex;

		CommandPool* commandPool;

	};
}

