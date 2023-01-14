/*************************
 * @file lyra.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief forward declare all core members and utility
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <glm.hpp>

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

// color type
struct Color {
	constexpr Color() = default;
	constexpr Color(uint8 r, uint8 g, uint8 b, uint8 a) : r(r), g(g), b(b), a(a) { }
	constexpr Color(const Color& col) : r(col.r), g(col.g), b(col.b), a(col.a) { }
	constexpr Color& operator=(const Color& col) { r = col.r; g = col.g; b = col.b; a = col.a; return *this; }
	constexpr glm::vec4 vec() const { return glm::vec4(r, g, b, a); }
	uint8 r, g, b, a;
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

// nodes

struct Script;
class Spatial;

class Mesh;
class MeshRenderer;
class Text;
class TextRenderer;

class Rigidbody;
class Collider;
class BoxCollider;
class SphereCollider;
class CapsuleCollider;
class TaperedCapsuleCollider;
class CylinderCollider;
class MeshCollider;
class Joint;
class Cloth;
class Raycast;

class Camera;
class PointLight;
class DirectionalLight;
class SpotLight;
class PointLight;
class AmbientLight;
class LightProbe;
class Cubemap;
class Skybox;
class MoviePlayer;

class ParticleSystem;
class LineRenderer;

class Animation;
class Animator;

class AudioSource;
class AudioListener;
class AudioFilter;

} // namespace lyra

// utility macros

#define NODISCARD [[nodiscard]]
#define FUNC_PTR(func) [&] { func }
#define TO_FUNC_PTR(func, type) type(*)(func*)

#define GLM_FORCE_RADIANS

#include <core/logger.h>
#include <core/util.h>
