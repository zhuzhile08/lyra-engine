/*************************
 * @file RAIIContainers.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief A vulkan RAII container template for storing and managing handles
 * 
 * @date 2023-02-24
 * 
 * @copyright Copyright (c) 2023
 ************************/

#pragma once

#include <type_traits>
#include <algorithm>
#include <string_view>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vulkan/vulkan.h>
#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wdeprecated-copy"
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#pragma clang diagnostic ignored "-Wswitch"
#include <vk_mem_alloc.h>
#pragma clang diagnostic pop
#else
#include <vk_mem_alloc.h>
#endif

#include <Common/Logger.h>

namespace lyra {

// a type in case the parent handle does not exist
using NullHandle = std::nullptr_t;

/**
 * @brief concept for the vulkan handle RAII container
 * 
 * @tparam Ty handle type
 */
template <class Ty> concept RAIIContainerType = 
	std::is_copy_constructible_v<Ty> && 
	std::is_copy_assignable_v<Ty> && 
	std::is_move_constructible_v<Ty> &&
	std::is_move_assignable_v<Ty>;

/**
 * @brief A RAII container for making the usage of vulkan handles more secure
 * 
 * @tparam Ty 
 * @tparam OTy 
 */
template <RAIIContainerType Ty, RAIIContainerType OTy> class RAIIContainer {
public:
	using handle = Ty;
	using const_handle = const handle;
	using handle_reference = Ty&;
	using const_handle_reference = const_handle&;
	using movable_handle = Ty&&;
	
	using owner = OTy;
	using const_owner = const owner;
	using owner_reference = owner&;
	using const_owner_reference = const_owner&;
	using movable_owner = owner&&;

	using container = RAIIContainer;
	using movable_container = container&&;

	constexpr RAIIContainer() = default;
	/**
	 * @brief construct a new RAII vulkan handle container
	 * 
	 * @param handle vulkan handle
	 * @param owner handle owner
	 */
	constexpr RAIIContainer(const_handle_reference handle, const_owner_reference owner) : m_handle(handle), m_owner(owner) { }
	/**
	 * @brief construct a new RAII vulkan handle container
	 * 
	 * @param handle vulkan handle
	 * @param owner handle owner
	 */
	constexpr RAIIContainer(movable_handle handle, movable_owner owner) : m_handle(std::move(handle)), m_owner(std::move(owner)) { }
	/**
	 * @brief construct a new RAII vulkan handle container
	 * 
	 * @param container container to construct from
	 */
	constexpr RAIIContainer(movable_container container) : 
		m_handle(std::exchange(container.m_handle, handle { } )), 
		m_owner(std::exchange(container.m_owner, owner { } )) { }
	/**
	 * @brief construct a new RAIIContainer 
	 * 
	 * @tparam CI creation information type
	 * @param owner owner
	 * @param createInfo creation information
	 */
	template <class CI> constexpr RAIIContainer(owner owner, const CI& createInfo) : RAIIContainer<Ty, OTy>(Ty { }, owner) { 
		if (!this->m_handle) {
			if constexpr (std::same_as<handle, VkFramebuffer>) {
				vassert(vkCreateFramebuffer(this->m_owner, &createInfo, nullptr, &this->m_handle), "create framebuffer");
			} else if constexpr (std::same_as<handle, VkRenderPass>) {
				vassert(vkCreateRenderPass(this->m_owner, &createInfo, nullptr, &this->m_handle), "create render pass");
			} else if constexpr (std::same_as<handle, VkSampler>) {
				vassert(vkCreateSampler(this->m_owner, &createInfo, nullptr, &this->m_handle), "create image sampler");
			} else if constexpr (std::same_as<handle, VkCommandPool>) {
				vassert(vkCreateCommandPool(this->m_owner, &createInfo, nullptr, &this->m_handle), "create command pool");
			} else if constexpr (std::same_as<handle, VkDescriptorSetLayout>) {
				vassert(vkCreateDescriptorSetLayout(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor set layout");
			} else if constexpr (std::same_as<handle, VkDescriptorPool>) {
				vassert(vkCreateDescriptorPool(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor pool");
			} else if constexpr (std::same_as<handle, VkDescriptorSet>) {
				vassert(vkAllocateDescriptorSets(this->m_owner, &createInfo, &this->m_handle), "allocate descriptor sets");
			} else if constexpr (std::same_as<handle, VkSemaphore>) {
				vassert(vkCreateSemaphore(this->m_owner, &createInfo, nullptr, &this->m_handle), "create semaphore");
			} else if constexpr (std::same_as<handle, VkFence>) {
				vassert(vkCreateFence(this->m_owner, &createInfo, nullptr, &this->m_handle), "create fence");
			} else if constexpr (std::same_as<handle, VkImageView>) {
				vassert(vkCreateImageView(this->m_owner, &createInfo, nullptr, &this->m_handle), "create image view");
			} else if constexpr (std::same_as<handle, VkPipelineLayout>) {
				vassert(vkCreatePipelineLayout(this->m_owner, &createInfo, nullptr, &this->m_handle), "create pipeline layout");
			} else if constexpr (std::same_as<handle, VkShaderModule>) {
				vassert(vkCreateShaderModule(this->m_owner, &createInfo, nullptr, &this->m_handle), "create shader module");
			} else if constexpr (std::same_as<handle, VkSwapchainKHR>) {
				vassert(vkCreateSwapchainKHR(this->m_owner, &createInfo, nullptr, &this->m_handle), "create instance");
			} else if constexpr (std::same_as<handle, VmaAllocator>) {
				vassert(vmaCreateAllocator(&createInfo, &this->m_handle), "create memory allocator");
			} else if constexpr (std::same_as<handle, VkDevice>) {
				vassert(vkCreateDevice(this->m_owner, &createInfo, nullptr, &this->m_handle), "create logical device");
			} else if constexpr (std::same_as<handle, VkInstance>) {
				vassert(vkCreateInstance(&createInfo, nullptr, &this->m_handle), "create instance");
			}
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is a Buffer or Image
	 * 
	 * @tparam CI creation information type
	 * @param owner owner
	 * @param allocator VMA memory allocator
	 * @param createInfo creation information
	 * @param allocCreateInfo VMA allocation/memory creation information
	 * @param allocation VMA allocation/memory
	 * @param allocInfo further information about the VMA allocation/memory
	 */
	constexpr RAIIContainer(
		owner owner, 
		VmaAllocator allocator, 
		const VkBufferCreateInfo& createInfo, 
		const VmaAllocationCreateInfo& allocCreateInfo, 
		RAIIContainer<VmaAllocation, VmaAllocator>& allocation, 
		VmaAllocationInfo allocInfo = VmaAllocationInfo { } 
	) requires(std::is_same_v<handle, VkBuffer> && std::is_same_v<owner, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			VmaAllocation tempAllocation;
			vassert(vmaCreateBuffer(allocator, &createInfo, &allocCreateInfo, &m_handle, &tempAllocation, &allocInfo), "create buffer and/or its memory");
			allocation = RAIIContainer<VmaAllocation, VmaAllocator>(tempAllocation, allocator);
		}
	}

	/**
	 * @brief construct a new RAIIContainer if the internal handle is a Buffer or Image
	 * 
	 * @param owner owner
	 * @param allocator VMA memory allocator
	 * @param createInfo creation information
	 * @param allocCreateInfo VMA allocation/memory creation information
	 * @param allocation VMA allocation/memory
	 * @param allocInfo further information about the VMA allocation/memory
	 */
	constexpr RAIIContainer(
		owner owner, 
		VmaAllocator allocator, 
		const VkImageCreateInfo& createInfo, 
		const VmaAllocationCreateInfo& allocCreateInfo, 
		RAIIContainer<VmaAllocation, VmaAllocator>& allocation, 
		VmaAllocationInfo allocInfo = VmaAllocationInfo { } 
	) requires(std::is_same_v<handle, VkImage> && std::is_same_v<owner, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			VmaAllocation tempAllocation;
			vassert(vmaCreateImage(allocator, &createInfo, &allocCreateInfo, &m_handle, &tempAllocation, &allocInfo), "create image and/or its memory");
			allocation = RAIIContainer<VmaAllocation, VmaAllocator>(tempAllocation, allocator);
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the handle is a graphics pipeline
	 * 
	 * @param owner owner
	 * @param pipelineCache pipeline cache
	 * @param createInfo creation information for the pipeine
	 * @param allocator allocator
	 */
	constexpr RAIIContainer(
		owner owner, 
		VkPipelineCache pipelineCache, 
		const VkGraphicsPipelineCreateInfo& createInfo, 
		const VkAllocationCallbacks& allocator
	) requires(std::is_same_v<handle, VkPipeline> && std::is_same_v<owner, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			vassert(vkCreateGraphicsPipelines(m_owner, pipelineCache, 1, &createInfo, nullptr, &m_handle), "create graphics pipeline");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the handle is a compute pipeline
	 * 
	 * @param owner owner
	 * @param pipelineCache pipeline cache
	 * @param createInfo creation information for the pipeine
	 * @param allocator allocator
	 */
	constexpr RAIIContainer(
		owner owner, 
		VkPipelineCache pipelineCache, 
		const VkComputePipelineCreateInfo& createInfo, 
		const VkAllocationCallbacks& allocator
	) requires(std::is_same_v<handle, VkPipeline> && std::is_same_v<owner, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			vassert(vkCreateComputePipelines(m_owner, pipelineCache, 1, &createInfo, nullptr, &m_handle), "create compute pipeline");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is a VkQueue
	 * 
	 * @param owner owner
	 * @param familyIndex index of the queue family
	 * @param queueIndex index of the queue
	 */
	constexpr RAIIContainer(
		owner owner, 
		const uint32_t& familyIndex, 
		const uint32_t& queueIndex)
		requires(std::is_same_v<handle, VkQueue> && std::is_same_v<owner, VkDevice>)
		 : m_owner(owner) {
		if (!m_handle) {
			vkGetDeviceQueue(m_owner, familyIndex, queueIndex, &m_handle);
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is a VkQueue
	 * 
	 * @param owner owner
	 * @param window window
	 */
	constexpr RAIIContainer(owner owner, SDL_Window* window) 
		requires(std::is_same_v<handle, VkSurfaceKHR> && std::is_same_v<owner, VkInstance>)
		 : m_owner(owner) {
		if (!m_handle) {
			SDL_Vulkan_CreateSurface(window, this->m_owner, &this->m_handle);
		}
	}
	
	constexpr RAIIContainer(
		std::string_view title, 
		const int& x, 
		const int& y, 
		const int& w, 
		const int& h, 
		const uint32_t& flags
	) requires(std::is_same_v<handle, SDL_Window*>) {
		if (!m_handle) {
			m_handle = SDL_CreateWindow(title.data(), x, y, w, h, flags);
		}
	}

	/**
	 * @brief destory the contained handle explicitly
	 */
	void destroy() {
		if (m_handle && static_cast<bool>(m_owner)) {
			if constexpr (std::same_as<owner, VkDevice>) {
				if constexpr (std::same_as<handle, VkFramebuffer>) {
					vkDestroyFramebuffer(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkRenderPass>) {
					vkDestroyRenderPass(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkSampler>) {
					vkDestroySampler(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkCommandPool>) {
					vkDestroyCommandPool(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkDescriptorSetLayout>) {
					vkDestroyDescriptorSetLayout(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkDescriptorPool>) {
					vkDestroyDescriptorPool(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkSemaphore>) {
					vkDestroySemaphore(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkFence>) {
					vkDestroyFence(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkBuffer>) {
					vkDestroyBuffer(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkImage>) {
					vkDestroyImage(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkImageView>) {
					vkDestroyImageView(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkPipeline>) {
					vkDestroyPipeline(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkPipelineLayout>) {
					vkDestroyPipelineLayout(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkShaderModule>) {
					vkDestroyShaderModule(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle, VkSwapchainKHR>) {
					vkDestroySwapchainKHR(m_owner, m_handle, nullptr);
				} 
			} else if constexpr (std::same_as<owner, VkInstance>) {
				if constexpr (std::same_as<handle, VkSurfaceKHR>) {
					vkDestroySurfaceKHR(m_owner, m_handle, nullptr);
				}
			} else if constexpr (std::same_as<owner, VmaAllocator>) {
				if constexpr (std::same_as<handle, VmaAllocation>) {
					vmaFreeMemory(m_owner, m_handle);
				}
			} else if constexpr (std::same_as<owner, VkCommandPool>) {
				if constexpr (std::same_as<handle, VkCommandBuffer>) {
					vkFreeCommandBuffers(m_owner, m_handle);
				}
			} else if constexpr (std::same_as<handle, VmaAllocator>) {
				vmaDestroyAllocator(m_handle);
			} else if constexpr (std::same_as<handle, VkDevice>) {
				vkDestroyDevice(m_handle, nullptr);
			} else if constexpr (std::same_as<handle, VkInstance>) {
				vkDestroyInstance(m_handle, nullptr);
			} else if constexpr (std::same_as<handle, SDL_Window*>) {
				SDL_DestroyWindow(m_handle);
			} // some objects don't need to be destroyed explicitly, so they aren't here, even though they use this RAII container
		}
	}
	/**
	 * @brief destroy the contained handle, basically only calls destroy
	 */
	virtual ~RAIIContainer() {
		destroy();
	}

	/**
	 * @brief move assignment operator
	 * 
	 * @param handle other handle
	 * @return lyra::vulkan::RAIIContainer& 
	 */
	constexpr RAIIContainer& operator=(movable_handle handle) {
		if (handle != this->m_handle) {
			m_handle = std::exchange(handle, handle { } );
		}
		return *this;
	}
	/**
	 * @brief move assignment operator
	 * 
	 * @param container other container
	 * @return lyra::vulkan::RAIIContainer& 
	 */
	constexpr RAIIContainer& operator=(movable_container container) {
		if (&container != this) {
			m_handle = std::exchange(container.m_handle, handle { } );
			m_owner = std::exchange(container.m_owner, owner { } );
		}
		return *this;
	}

	/**
	 * @brief get the raw vulkan handle
	 * 
	 * @return lyra::vulkan::RAIIContainer::handle&
	 */
	constexpr handle& get() noexcept { return m_handle; }
	/**
	 * @brief get the raw vulkan handle
	 * 
	 * @return lyra::vulkan::RAIIContainer::const_handle&
	 */
	constexpr const_handle& get() const noexcept { return m_handle; }
	/**
	 * @brief get the raw vulkan handle
	 * 
	 * @return lyra::vulkan::RAIIContainer::handle&
	 */
	constexpr operator handle&() noexcept { return m_handle; }
	/**
	 * @brief get the raw vulkan handle
	 * 
	 * @return lyra::vulkan::RAIIContainer::const_handle&
	 */
	constexpr operator const_handle&() const noexcept { return m_handle; }

protected:
	handle m_handle { };
	owner m_owner { };
};

namespace vulkan {

namespace vk {

using Instance = RAIIContainer<VkInstance, NullHandle>;
using PhysicalDevice = RAIIContainer<VkPhysicalDevice, VkInstance>;
using Device = RAIIContainer<VkDevice, VkPhysicalDevice>;
using Queue = RAIIContainer<VkQueue, VkDevice>;
using SurfaceKHR = RAIIContainer<VkSurfaceKHR, VkInstance>;
using Surface = SurfaceKHR;
using CommandPool = RAIIContainer<VkCommandPool, VkDevice>;
using SwapchainKHR = RAIIContainer<VkSwapchainKHR, VkDevice>;
using Swapchain = SwapchainKHR;
using Image = RAIIContainer<VkImage, VkDevice>;
using ImageView = RAIIContainer<VkImageView, VkDevice>;
using RenderPass = RAIIContainer<VkRenderPass, VkDevice>;
using Framebuffer = RAIIContainer<VkFramebuffer, VkDevice>;
using Semaphore = RAIIContainer<VkSemaphore, VkDevice>;
using Fence = RAIIContainer<VkFence, VkDevice>;
using DescriptorSetLayout = RAIIContainer<VkDescriptorSetLayout, VkDevice>;
using DescriptorPool = RAIIContainer<VkDescriptorPool, VkDevice>;
using DescriptorSet = RAIIContainer<VkDescriptorSet, VkDevice>;
using ShaderModule = RAIIContainer<VkShaderModule, VkDevice>;
using PipelineLayout = RAIIContainer<VkPipelineLayout, VkDevice>;
using Pipeline = RAIIContainer<VkPipeline, VkDevice>;
using GraphicsPipeline = Pipeline;
using ComputePipeline = Pipeline;
using Buffer = RAIIContainer<VkBuffer, VkDevice>;
using Sampler = RAIIContainer<VkSampler, VkDevice>;

} // namespace handles

namespace vma {

using Allocator = RAIIContainer<VmaAllocator, VkInstance>;
using Allocation = RAIIContainer<VmaAllocation, VmaAllocator>;

} // namespace vma

} // namespace vulkan

namespace sdl {

using Window = RAIIContainer<SDL_Window*, NullHandle>;

}

} // namespace lyra