#include "CommandBuffer.h"


namespace one{
	CommandBuffer::CommandBuffer(VkDevice _device, VkCommandPool _commandPool) : _device(_device), _commandPool(_commandPool){
		initialize();
	}

	void CommandBuffer::initialize() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = _commandPool;
		//primary can be go to queue execution, but cant get called from other buffers
		//secondary can not go to queue execution, but can get called from other buffers
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffer!");
		}
	}

	//writes commands to execute in command buffer
	//in this case write to image
	void CommandBuffer::recordCommandBuffer(VkFramebuffer frameBuffer, VkRenderPass renderPass, VkPipeline graphicsPipeline, VkExtent2D swapChainExtent) {

		//start by specifying details on usage of such
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//how to use this command buffer options: record right after executing(one time)
		//secondary command on a single render pass
		//can be resubmitted while it has already been(multiple times)
		beginInfo.flags = 0;
		//only for secondary buffers, inherts states from one who called
		beginInfo.pInheritanceInfo = nullptr;

		//if it has been called calling it again will reset it not change it
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		//starting renderpass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		//renderpass will bind framebuffer to swapchain image
		renderPassInfo.framebuffer = frameBuffer;
		//shader loads and stores will take place in render area(should be same size as attchments)
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;
		//clear values for atachment LOAD_OP_CLEAR for color attchment
		VkClearValue clearColor = { {{0.0f,0.0f,0.0f,1.0f}} };//black 100% opacity
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		//renderpass has begun
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);//last specifies commands are primary/other option sets them to come from secondary

		//specifies its graphics and not compute 
		//this just told vulkan wich operations to execute and which attachments to use(in fragment shader)
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		//viewport and scissor are dynamic, need to specify before drawing
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		//now the draw command for the triangle
		//info besides cmdBuffer
		//vertexCount = even though no vertex buffer it will draw 3 vertex
		//instanceCount = used for instance rendering, 1 if not doing that
		//firstVertex = offset into the vertex buffer, lowest value of VertexIndex
		//firstInstance = offset into the instance rendering, lowest value of InstanceIndex
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		//the renderPass can now be ended
		vkCmdEndRenderPass(commandBuffer);

		//finish command buffer
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

	}

	bool CommandBuffer::reset() {
		return vkResetCommandBuffer(commandBuffer, 0) == VK_SUCCESS;
	}

	void CommandBuffer::destroy() {

	}
	CommandBuffer::~CommandBuffer() {

	}
}
