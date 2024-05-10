/*************************
 * @file Common.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief forward declare all core members and utility
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <stddef.h>
#include <stdint.h>
#include <cassert>

#include <glm/glm.hpp>

namespace lyra {

// abbreviations for fixed width integers

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using uchar = unsigned char; // mostly alternative names, borrowed from ogre
using ushort = unsigned short;
using ulong = unsigned long;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

// floating point types

using float32 = float;
using float64 = double;
using float128 = long double; // not supported everywhere, so don't really use this

// some other common types

using wchar = wchar_t;
using filepos = fpos_t;
using object_id = uint64;
using uintptr = std::uintptr_t;
using nullpointer = decltype(nullptr);
using size_type = std::size_t;
using type_id = const void*;

// color type
struct Color {
	constexpr Color() = default;
	constexpr Color(float32 r, float32 g, float32 b, float32 a = 1.0f) : r(r), g(g), b(b), a(a) { }
	constexpr Color(const Color& col) : r(col.r), g(col.g), b(col.b), a(col.a) { }
	constexpr Color& operator=(const Color& col) { r = col.r; g = col.g; b = col.b; a = col.a; return *this; }
	GLM_CONSTEXPR glm::vec4 vec() const { return glm::vec4(r, g, b, a); }
	float32 r { }, g { }, b { }, a { };
};
using Colour = Color;

// angle types

struct Radian {
	float32 r;

	constexpr operator float32() const noexcept {
		return r;
	}
	constexpr operator float32&() noexcept {
		return r;
	}
};


// Hash function

template <class Ty> struct Hash;


// some higher level wrappers

class Logger;
class Window;
class Material;
class Texture;

// basic rendering patterns

class BasicRenderer;
class UnorderedBasicRenderer;
class OrderedBasicRenderer;
class VectorBasicRenderer;
class RenderObject;

// core vulkan wrappers

namespace vulkan {

class CommandQueue;
class GPUMemory;
class GPUBuffer;
class Image;
class Swapchain;
class RenderTarget;
class Shader;
class DescriptorSets;
class GraphicsProgram;
class GraphicsPipeline;
class ComputeProgram;
class ComputePipeline;
class DescriptorPools;
class ImGuiRenderer;

} // namespace vulkan

class ImGuiRenderer;

// entities and components

class Entity;

class Transform;

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

// assert function forward declarations

#ifdef NDEBUG
template <class Cond> inline constexpr void conditionToExpression(const Cond&) { }
#endif
template <class Msg> inline constexpr void lyraAssert(bool, Msg);
template <class Msg> inline constexpr void vulkanAssert(int, Msg);
template <class Format, typename ... Args> inline constexpr void lyraAssert(bool, Format&&, Args&&...);
template <class Format, typename ... Args> inline constexpr void vulkanAssert(int, Format&&, Args&&...);

} // namespace lyra

// utility macros

#ifndef NDEBUG
#define ASSERT(...) lyra::lyraAssert(__VA_ARGS__)
#define VULKAN_ASSERT(...) lyra::vulkanAssert(__VA_ARGS__)
#else
#define ASSERT(cond, ...) conditionToExpression(cond);
#define VULKAN_ASSERT(cond, ...) conditionToExpression(cond);
#endif

#define NODISCARD [[nodiscard]]
#define DEPRECATED [[deprecated]]
#define NO_UNIQUE_ADDRESS [[no_unique_address]]
#define FUNC_PTR(func) [&] { func }
#define TO_FUNC_PTR(func, type) type(*)(func*)
#define DEFINE_DEFAULT_MOVE(type) type(type&&) = default; type& operator=(type&&) = default;

#define GLM_FORCE_RADIANS

#ifdef _WIN32
#define WIN32_CONSTEXPR
#else
#define WIN32_CONSTEXPR constexpr
#endif
