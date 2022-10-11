/*************************
 * @file decl.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief forward declare all core members
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <stdint.h>

namespace lyra {

// abbreviations for fixed width integers

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

// pointer types

typedef uintptr_t uptr;

// color
struct Color {
	uint8 r = 0, g = 0, b = 0, a = 0;
};

typedef Color Colour;

// core vulkan wrappers

namespace vulkan {

class Device;
class Window;
class CommandPool;
class CommandBufferManager;
class DescriptorSetLayout;
class DescriptorPool;
class Descriptor;
struct GPUMemory;
struct Image;
class GPUBuffer;
class Shader;
class Pipeline;

} // namespace vulkan

// some higher level wrappers

class Window;
class Renderer;
class RenderSystem;
class GraphicsPipeline;
class Material;
class Texture;

namespace gui {

class GUIRenderer;

} // namespace gui

// low level utility

class CallQueue;
class BaseAllocator;
class LinearAllocator;
class StackAllocator;
class PoolAllocator;

// utility macros

#define NODISCARD [[nodiscard]]
#define FUNC_PTR(func) [&] { func }
#define TO_FUNC_PTR(func, type) type(*)(func*)

} // namespace lyra
