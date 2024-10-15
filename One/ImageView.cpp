#include "ImageView.h"

namespace one {
	ImageView::ImageView(VkDevice _device, VkImage _image, VkFormat swapchainImageFormat) :  _image(_image){
		initialize(_device, swapchainImageFormat);
	}

	void ImageView::initialize(VkDevice _device, VkFormat swapchainImageFormat) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _image;
		//could be 1D textures, 2D textures, 3D textures and cube maps.
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainImageFormat;

		//can map the color channels differently( exp. monochrome = allpoitn to one))
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		//
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;//purpose
		createInfo.subresourceRange.baseMipLevel = 0;//mipmapping levels
		createInfo.subresourceRange.levelCount = 1;//?
		createInfo.subresourceRange.baseArrayLayer = 0;//purpose
		createInfo.subresourceRange.layerCount = 1;//multiple layers per view

		if (vkCreateImageView(_device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}

		std::cerr << "vulkan imageview has initiated \n";
	}
	void ImageView::destroy(VkDevice _device) {
		if (imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(_device, imageView, nullptr);
			imageView = VK_NULL_HANDLE;
		}
	}
	ImageView::~ImageView() {
		//no need as no problems emerged and I prefer not saving a handle of _device in the class yet
	}
}