#include "RenderPass.h"

namespace one {
	RenderPass::RenderPass(VkDevice _device, VkFormat _swapchainImageFormat) : _device(_device) {
		initialize(_swapchainImageFormat);
	}

	//frameBuffer and renderring recommendations:
	//only put the necessary information that will be processed on framebuffer
	//pay a lot of attention to load ops and store ops
	//put all independent work items(same resolution) in the same renderpass
	//if able use by_region dependencies between subpasses

	//SubPasses are rendering tasks done in some oreder
	//Subpasses have inputs and outputs / there are shared dependencies between subpasses
	//example: SubPass0 wrote to depth buffer
	//			SubPass1 read from depth buffer and wrote to color buffer
	//			Subpass2 read from color buffer and wrote to framebuffer
	//The renderpass just specifies the states you need each attachment to be before Subpasses
	void RenderPass::initialize(VkFormat _swapchainImageFormat) {
		//one color buffer attachment to one image
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = _swapchainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;//no multisampling yet
		//what to do with data before rendering /ops: preserve previous attchments ; clear them ; dont care about them
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//about color and depth
		//what to do after / ops: store them after render to use; discard them
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//about color and depth
		//2 next about stencil buffer which is just an extra data for things like shadows or outlines
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//how pixels of the images aresupposed to be arranged. matches the operation being made at stage
		//as: color attachments ; present to swapchain ; destination for copy operations
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//before render pass
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//right after render finishes


		//Subpasses -  rendering operations that depend on frambuffer previous passes(for now just 1)
		//putting in a single render pass allows vulkan to optimize it
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;//index in attachment description array(for now just one so index 0)
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//layout during subpass
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		//this is the index used in "location": layout(location = 0) out vec4 outColor
		subpass.pColorAttachments = &colorAttachmentRef;
		//other ones needed in the future: 
		// pInputAttachments(read from shader) 
		// pResolveAttachments(multisampling color)
		// pDepthStencilAttachment(depth and Stencil data)
		// pPreserveAttachments(not used for this subpass but has to be passed on)

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		//dependencies are the info about which subpass needs which to be done so the tasks cna be executed and who needs them
		VkSubpassDependency dependency{};
		//src must be bigger than dst
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;//source - where it is coming from
		dependency.dstSubpass = 0;//destination - where it is referring to (we just have one so 0)
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//pass where they occur
		dependency.srcAccessMask = 0;//operations to wait on(we are waiting on whole stage itself so imgae to finish being read from on swapchain)
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;//pass where they occur
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;//operations waiting(in this case waiting to write)

		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

	}
	void RenderPass::destroy() {
		if (renderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(_device, renderPass, nullptr);
			renderPass = VK_NULL_HANDLE;
		}
	}
	RenderPass::~RenderPass() {
		destroy();
	}
}