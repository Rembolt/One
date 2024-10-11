#pragma once
#include "UtilHeader.h"
#include "CommandPool.h"
#include "Fence.h"

namespace one {
	class Queue : NonCopyable
	{
	public:

		Queue(uint32_t familyIndex, float priority);
		~Queue();

		bool initialize(VkDevice _device);
		void destroy();
		void initializeCommandPool();
		void submit(VkSubmitInfo submitInfo, Fence* fence);
		void present(VkPresentInfoKHR presentInfo);

		inline VkQueue getQueue() const{
			return queue;
		}

		inline uint32_t getFamilyIndex() const {
			return familyIndex;
		}

		inline float* getPriority(){
			return &priority;
		}

		inline bool setFamilyIndex(uint32_t index) {
			 familyIndex = index;
			 return true;
		}

		inline bool setQueueCount(uint16_t count) {
			queueCount = count;
			return true;
		}

		inline VkCommandPool getCommandPool(void) const {
			return pCommandPool->getCommandPool();
		}


	private:

		VkDevice _device;

		VkQueue queue;
		uint32_t familyIndex;
		uint16_t queueCount;
		float priority;

		CommandPool* pCommandPool;

	};
}

