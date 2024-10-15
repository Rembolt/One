#pragma once
#include "NonCopyable.h"//stops class from being copied because of vulkan 
#include <vector>
#define GLFW_INCLUDE_VULKAN //GLFW has its own definitions for vulkan and will call it
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>
#include <cassert>

// check if c++ is compiling in anything other than debug mode
#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYER = false;
#else
const bool ENABLE_VALIDATION_LAYER = true;
#endif