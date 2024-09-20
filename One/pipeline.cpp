#include "pipeline.h"
#include <fstream>
#include <vector>
#include <iostream>

namespace one {
	Pipeline::Pipeline(){}

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
		std::cerr << "pipeline has initiated \n";
	}

	Pipeline::~Pipeline() {}
}


