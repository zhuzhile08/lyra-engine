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

template <class> class Script;
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
class Light;
class LightProbe;
class Skybox;
class MoviePlayer;

class ParticleSystem;
class LineRenderer;

class Animation;
class Animator;

class AudioSource;
class AudioListener;
class AudioFilter;

// easily define a script member for any class
#define LYRA_NODE_SCRIPT_MEMBER(class) lyra::SmartPointer<lyra::Script<class>> m_script; // please don't kill me C++ gods

} // namespace lyra

// utility macros

#define NODISCARD [[nodiscard]]
#define CONSTEXPR constexpr
#define FUNC_PTR(func) [&] { func }
#define TO_FUNC_PTR(func, type) type(*)(func*)

#include <core/logger.h>
#include <core/util.h>
