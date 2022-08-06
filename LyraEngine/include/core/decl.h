/*************************
 * @file decl.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief forward declare all core members
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

namespace lyra {

// abbreviations for fixed width integers

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

// pointer types

typedef uintptr_t uptr;

// vulkan command buffer index type

typedef uint8_t CommandBuffer;

// color
struct Color {
	uint8 r = 0, g = 0, b = 0, a = 0;
};

typedef Color Colour;

// core vulkan wrappers

class VulkanDevice;
class VulkanWindow;
class VulkanCommandPool;
class CommandBufferManager;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;
class VulkanDescriptor;
struct VulkanGPUMemory;
struct VulkanImage;
class VulkanGPUBuffer;
class VulkanShader;
class VulkanPipeline;

// some higher level wrappers

class Window;
class Renderer;
class RenderSystem;
class GraphicsPipeline;
class GUIRenderer;
class Material;
class Texture;

// low level utility

class CallQueue;
class BaseAllocator;
class LinearAllocator;
class StackAllocator;
class PoolAllocator;

} // namespace lyra
