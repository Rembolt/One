#include "Pipeline.h"
#include <fstream>


namespace one {
	Pipeline::Pipeline(VkDevice _device, VkRenderPass _renderPass): _device(_device){
		initialize(_renderPass);
	}

	//read binary data from file
	static std::vector<char> readFile(const std::string& filename) {
		//ate:read from end
		//binary:read as binary
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();//gets position of file reader and transforms to size
		std::vector<char> buffer(fileSize);//allocates buffer of filesize
		file.seekg(0);//set reader to beginning
		file.read(buffer.data(), fileSize);//reads at once
		file.close();
		return buffer;
	}

	void Pipeline::initialize(VkRenderPass _renderPass) {
		auto vertShaderCode = readFile("shader.vert.spv");
		auto fragShaderCode = readFile("shader.frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		//assigns shader to pipeline stage(reference to pipeline diagram)
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		//function to invoke or ENTRYPOINT
		//that means we can combine multiple fragment shader(exp.) in a single module
		vertShaderStageInfo.pName = "main";
		//enumerator bellow allows to change values for shader constants like fov(exp.) 
		//which when compiled optimize the code making it faster(ignores if's (exp.)
		//only worth it when change is big and comes from "setting" for example
		//vertShaderStageInfo.pSpecializationInfo 

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { 
			vertShaderStageInfo, fragShaderStageInfo };

		//*************************************************************************************
		//Specifies BIndings(or if data is per vertex or per instance)
		//instance is when a single mesh is duplicated and we refer to each type of duplicate by instance
		//for now vertexes are hardcoded(change when buffers come in)
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;//array of structs holding detail to load vertex data
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;//array of structs holding detail to load vertex data

		//*************************************************************************************
		//What kind of geometry and if primitive restart is enabled
		//basically how to draw triangle from list of vertices
		//DEFINETLY come back here to try to optimize the way the voxel sides are drawn with triangles
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//every 3 vertices without reuse(not optimized)
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		//*************************************************************************************
		// these can be included on the dynamic part of the viewport
		//size of images we are drawing to and depth values
		//VkViewport viewport{};
		//viewport.x = 0.0f;
		//viewport.y = 0.0f;
		//viewport.width = (float)m_app.swapChainExtent.width;
		//viewport.height = (float)m_app.swapChainExtent.height;
		//viewport.minDepth = 0.0f;
		//viewport.minDepth = 1.0f;
		////scissors are the part of te viewport in which we store any data(meaning will be rendered)
		//VkRect2D scissor{};
		//scissor.offset = {0,0};
		//scissor.extent = m_app.swapChainExtent;
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;//usingmultiple requires to set a GPU freature
		viewportState.scissorCount = 1;//check if I can take scissor out(performance)
		/*viewportState.pViewports = &viewport;
		viewportState.pScissor = &scissor;*/ 

		//*************************************************************************************
		//Dynamic state part of the pipeline(we are taking them out of creation) 
		//so we can tweak them (required) specify at drawing time(command buffer)
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR	
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		//*************************************************************************************
		//takes geometry and turns it into fragments to be colored by fragment shader
		//performs depth testing / face culling / scissor test 
		//can be set to just render wireframe or entire polygon
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;	
		rasterizer.depthClampEnable = VK_FALSE;//if true clamps fragments outside planes in view(usefull for shadowmap)(requires GPU feature)
		rasterizer.rasterizerDiscardEnable = VK_FALSE;//geometry never passes through rasterizer stage
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;//any other mode  requires GPU feature
		rasterizer.lineWidth = 1.0f;//number of fragments in line(any thicker requires GPU feature
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;//sometimes used for shadow mapping
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;
		//*************************************************************************************
		//Multisampling:one way to antialising - combining multiple poligon that rasterize to same pixel
		//makes edges smooth
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;
		//*************************************************************************************
		//depth and stencil testing - VkPipelineDepthStencilStateCreateInfo
		//dont have one so pass nullptr;
		//*************************************************************************************
		//color blending - either mix old and new - or combine using bitwise operation (check vulkan spec)
		//for multiple frame buffers, there is per attached state color blend
		//and has to set up second one global color / for now just one frambe buffer so
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;	
		/*pseudocode to understand what above does
		if (blendEnable) {
			finalColor.rgb = (srcColorBlendFactor * newColor.rgb) < colorBlendOp > (dstColorBlendFactor * oldColor.rgb);
			finalColor.a = (srcAlphaBlendFactor * newColor.a) < alphaBlendOp > (dstAlphaBlendFactor * oldColor.a);
		}
		else {
			finalColor = newColor;
		}
		finalColor = finalColor & colorWriteMask;
		

		opacity version:
		//finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
		//finalColor.a = newAlpha.a;
		
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;*/
		
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;//true if applying any blend
		colorBlending.logicOp = VK_LOGIC_OP_COPY;//bitwise operation, disables every attached blend
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
		//*************************************************************************************
		//these uniform and push values in shaders are dynamic globals that can be passed
		//(at drwaing time)to modify shader behavior
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
		//*************************************************************************************
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;//shaderinfo structs
		//fixed function stages
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		//
		pipelineInfo.layout = pipelineLayout;
		//create renderpass setup info:
		//can use other renderpasses with this pipeline at runtime as long as they are compatible with this one
		pipelineInfo.renderPass = _renderPass;
		pipelineInfo.subpass = 0;
		//optional
		// we can create pipelines based on other pipelines to optimize things when switching between them
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;//VK_PIPELINE_CREATE_DERIVATIVE_BIT has to be active if so
		pipelineInfo.basePipelineIndex = -1;

		//null handle here is pipeline cache data to store to file or temporary memory so next pipeline creations are faster
		if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		//*************************************************************************************
		//we can destroy them as they have been passed to the pipeline and linked to the GPU already
		vkDestroyShaderModule(_device, fragShaderModule, nullptr);
		vkDestroyShaderModule(_device, vertShaderModule, nullptr);

		std::cerr << "vulkan pipeline has initiated \n";
	}


	//simply creates a wrapper arround shader byte code
	VkShaderModule Pipeline::createShaderModule(const std::vector<char>& shaderCode) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		//pointer must be uint32 pointer, but we have a char pointer, so we must re-interpret the pointer
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	void Pipeline::destroy() {
		if (pipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(_device, pipeline, nullptr);
			pipeline = VK_NULL_HANDLE;
		}
		if (pipelineLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(_device, pipelineLayout, nullptr);
			pipelineLayout = VK_NULL_HANDLE;
		}
	}

	Pipeline::~Pipeline() {
		destroy();
	}

}


