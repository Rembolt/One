#pragma once
#include "UtilHeader.h"

namespace one {
	class ImageView : NonCopyable
	{
	public:

		ImageView(VkDevice _device, VkImage _image, VkFormat swapchainImageFormat);
		~ImageView();

		void initialize(VkDevice _device, VkFormat swapchainImageFormat);
		void destroy();

		inline VkImageView getImageView(void) const {
			return imageView;
		}

	private:

		VkImageView imageView;

		//target image from swap chain
		VkImage _image;

	};
}