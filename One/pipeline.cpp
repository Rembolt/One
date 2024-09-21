#include "pipeline.h"
#include <fstream>

namespace one {
	Pipeline::Pipeline(App& app): m_app(app) {}

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

	void Pipeline::createPipeline() {
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
		//for now vertexes are hardcosed(change when buffers come in)
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
		//ended here(Multisampling)



		//*************************************************************************************
		//we can destroy them as they have been passed to the pipeline and linked to the GPU already
		vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
		vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);

		std::cerr << "pipeline has initiated \n";
	}

	//simply creates a wrapper arround shader byte code
	VkShaderModule Pipeline::createShaderModule(const std::vector<char>& shaderCode) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		//pointer must be uint32 pointer, but we have a char pointer, so we must re-interpret the pointer
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	Pipeline::~Pipeline() {}
}


