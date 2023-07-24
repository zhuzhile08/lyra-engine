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

#include <stddef.h>
#include <stdint.h>

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
using uintptr = uintptr_t;
using nullpointer = decltype(nullptr);
using size = size_t; // unreliable mostly because of size function in most containers, resort to 

// color type
struct Color {
	constexpr Color() = default;
	constexpr Color(float32 r, float32 g, float32 b, float32 a = 1.0f) : r(r), g(g), b(b), a(a) { }
	constexpr Color(const Color& col) : r(col.r), g(col.g), b(col.b), a(col.a) { }
	constexpr Color& operator=(const Color& col) { r = col.r; g = col.g; b = col.b; a = col.a; return *this; }
	constexpr glm::vec4 vec() const { return glm::vec4(r, g, b, a); }
	float32 r, g, b, a;
};
using Colour = Color;

// some higher level wrappers

class Window;
class Renderer;
class RenderSystem;
class GraphicsPipelineSystem;
class Material;
class Texture;

// core vulkan wrappers

namespace vulkan {

class Device;
class Window;
class CommandPool;
class CommandBufferManager;
class DescriptorSystem;
class GPUMemory;
class Image;
class GPUBuffer;
class Shader;
class Pipeline;

namespace detail {

struct VulkanWrapperBase {
	RenderSystem* renderSystem;
};

#define VULKAN_WRAPPER : private VulkanWrapper

} // namespace detail

} // namespace vulkan

namespace gui {

class GUIRenderer;

} // namespace gui

// entities and components

class Entity;
struct Script;
class ComponentBase;

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

} // namespace lyra

// utility macros

#define NODISCARD [[nodiscard]]
#define DEPRECATED [[deprecated]]
#define FUNC_PTR(func) [&] { func }
#define TO_FUNC_PTR(func, type) type(*)(func*)
#define DEFINE_DEFAULT_MOVE(type) type(type&&) = default; type& operator=(type&&) = default;

#define GLM_FORCE_RADIANS
