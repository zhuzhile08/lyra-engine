/*************************
 * @file vulkan_raii.h
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

#include <core/logger.h>

namespace lyra {

namespace vulkan {

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
	typedef Ty handle_type;
	typedef const handle_type const_handle_type;
	typedef Ty& handle_reference;
	typedef const_handle_type& const_handle_reference;
	typedef Ty&& movable_handle;
	
	typedef OTy owner_type;
	typedef const owner_type const_owner_type;
	typedef owner_type& owner_reference;
	typedef const_owner_type& const_owner_reference;
	typedef owner_type&& movable_owner;

	typedef RAIIContainer container_type;
	typedef container_type&& movable_container;

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
		m_handle(std::exchange(container.m_handle, handle_type { } )), 
		m_owner(std::exchange(container.m_owner, owner_type { } )) { }
	/**
	 * @brief construct a new RAIIContainer 
	 * 
	 * @tparam CI creation information type
	 * @param owner owner
	 * @param createInfo creation information
	 */
	template <class CI> constexpr RAIIContainer(owner_type owner, const CI& createInfo) : RAIIContainer<Ty, OTy>(Ty { }, owner) { 
		if (!this->m_handle) {
			if constexpr (std::same_as<handle_type, VkFramebuffer>) {
				vassert(vkCreateFramebuffer(this->m_owner, &createInfo, nullptr, &this->m_handle), "create framebuffer");
			} else if constexpr (std::same_as<handle_type, VkRenderPass>) {
				vassert(vkCreateRenderPass(this->m_owner, &createInfo, nullptr, &this->m_handle), "create render pass");
			} else if constexpr (std::same_as<handle_type, VkSampler>) {
				vassert(vkCreateSampler(this->m_owner, &createInfo, nullptr, &this->m_handle), "create image sampler");
			} else if constexpr (std::same_as<handle_type, VkCommandPool>) {
				vassert(vkCreateCommandPool(this->m_owner, &createInfo, nullptr, &this->m_handle), "create command pool");
			} else if constexpr (std::same_as<handle_type, VkDescriptorSetLayout>) {
				vassert(vkCreateDescriptorSetLayout(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor set layout");
			} else if constexpr (std::same_as<handle_type, VkDescriptorPool>) {
				vassert(vkCreateDescriptorPool(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor pool");
			} else if constexpr (std::same_as<handle_type, VkSemaphore>) {
				vassert(vkCreateSemaphore(this->m_owner, &createInfo, nullptr, &this->m_handle), "create semaphore");
			} else if constexpr (std::same_as<handle_type, VkFence>) {
				vassert(vkCreateFence(this->m_owner, &createInfo, nullptr, &this->m_handle), "create fence");
			} else if constexpr (std::same_as<handle_type, VkImageView>) {
				vassert(vkCreateImageView(this->m_owner, &createInfo, nullptr, &this->m_handle), "create image view");
			} else if constexpr (std::same_as<handle_type, VkPipelineLayout>) {
				vassert(vkCreatePipelineLayout(this->m_owner, &createInfo, nullptr, &this->m_handle), "create pipeline layout");
			} else if constexpr (std::same_as<handle_type, VkShaderModule>) {
				vassert(vkCreateShaderModule(this->m_owner, &createInfo, nullptr, &this->m_handle), "create shader module");
			} else if constexpr (std::same_as<handle_type, VkSwapchainKHR>) {
				vassert(vkCreateSwapchainKHR(this->m_owner, &createInfo, nullptr, &this->m_handle), "create instance");
			} else if constexpr (std::same_as<handle_type, VmaAllocator>) {
				vassert(vmaCreateAllocator(&createInfo, &this->m_handle), "create memory allocator");
			} else if constexpr (std::same_as<handle_type, VkDevice>) {
				vassert(vkCreateDevice(this->m_owner, &createInfo, nullptr, &this->m_handle), "create logical device");
			} else if constexpr (std::same_as<handle_type, VkInstance>) {
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
		owner_type owner, 
		VmaAllocator allocator, 
		const VkBufferCreateInfo& createInfo, 
		const VmaAllocationCreateInfo& allocCreateInfo, 
		RAIIContainer<VmaAllocation, VmaAllocator>& allocation, 
		VmaAllocationInfo allocInfo = VmaAllocationInfo { } 
	) requires(std::is_same_v<handle_type, VkBuffer> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
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
		owner_type owner, 
		VmaAllocator allocator, 
		const VkImageCreateInfo& createInfo, 
		const VmaAllocationCreateInfo& allocCreateInfo, 
		RAIIContainer<VmaAllocation, VmaAllocator>& allocation, 
		VmaAllocationInfo allocInfo = VmaAllocationInfo { } 
	) requires(std::is_same_v<handle_type, VkImage> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
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
		owner_type owner, 
		VkPipelineCache pipelineCache, 
		const VkGraphicsPipelineCreateInfo& createInfo, 
		const VkAllocationCallbacks& allocator
	) requires(std::is_same_v<handle_type, VkPipeline> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
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
		owner_type owner, 
		VkPipelineCache pipelineCache, 
		const VkComputePipelineCreateInfo& createInfo, 
		const VkAllocationCallbacks& allocator
	) requires(std::is_same_v<handle_type, VkPipeline> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
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
		owner_type owner, 
		const uint32_t& familyIndex, 
		const uint32_t& queueIndex)
		requires(std::is_same_v<handle_type, VkQueue> && std::is_same_v<owner_type, VkDevice>)
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
	constexpr RAIIContainer(owner_type owner, SDL_Window* window) 
		requires(std::is_same_v<handle_type, VkSurfaceKHR> && std::is_same_v<owner_type, VkInstance>)
		 : m_owner(owner) {
		if (!m_handle) {
			SDL_Vulkan_CreateSurface(window, this->m_owner, &this->m_handle);
		}
	}

	/**
	 * @brief destory the contained handle explicitly
	 */
	void destroy() {
		if (m_handle && static_cast<bool>(m_owner)) {
			if constexpr (std::same_as<owner_type, VkDevice>) {
				if constexpr (std::same_as<handle_type, VkFramebuffer>) {
					vkDestroyFramebuffer(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkRenderPass>) {
					vkDestroyRenderPass(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkSampler>) {
					vkDestroySampler(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkCommandPool>) {
					vkDestroyCommandPool(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkDescriptorSetLayout>) {
					vkDestroyDescriptorSetLayout(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkDescriptorPool>) {
					vkDestroyDescriptorPool(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkSemaphore>) {
					vkDestroySemaphore(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkFence>) {
					vkDestroyFence(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkBuffer>) {
					vkDestroyBuffer(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkImage>) {
					vkDestroyImage(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkImageView>) {
					vkDestroyImageView(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkPipeline>) {
					vkDestroyPipeline(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkPipelineLayout>) {
					vkDestroyPipelineLayout(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkShaderModule>) {
					vkDestroyShaderModule(m_owner, m_handle, nullptr);
				} else if constexpr (std::same_as<handle_type, VkSwapchainKHR>) {
					vkDestroySwapchainKHR(m_owner, m_handle, nullptr);
				} 
			} else if constexpr (std::same_as<owner_type, VkInstance>) {
				if constexpr (std::same_as<handle_type, VkSurfaceKHR>) {
					vkDestroySurfaceKHR(m_owner, m_handle, nullptr);
				}
			} else if constexpr (std::same_as<owner_type, VmaAllocator>) {
				if constexpr (std::same_as<handle_type, VmaAllocation>) {
					vmaFreeMemory(m_owner, m_handle);
				}
			} else if constexpr (std::same_as<owner_type, VkCommandPool>) {
				if constexpr (std::same_as<handle_type, VkCommandBuffer>) {
					vkFreeCommandBuffers(m_owner, m_handle);
				}
			} else if constexpr (std::same_as<handle_type, VmaAllocator>) {
				vmaDestroyAllocator(m_handle);
			} else if constexpr (std::same_as<handle_type, VkDevice>) {
				vkDestroyDevice(m_handle, nullptr);
			} else if constexpr (std::same_as<handle_type, VkInstance>) {
				vkDestroyInstance(m_handle, nullptr);
			}// some objects don't need to be destroyed explicitly, so they aren't here, even though they use this RAII container
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
            m_handle = std::exchange(handle, handle_type { } );
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
            m_handle = std::exchange(container.m_handle, handle_type { } );
            m_owner = std::exchange(container.m_owner, owner_type { } );
        }
		return *this;
	}

	/**
	 * @brief get the raw vulkan handle
	 * 
	 * @return lyra::vulkan::RAIIContainer::handle_type&
	 */
	constexpr handle_type& get() noexcept { return m_handle; }
	/**
	 * @brief get the raw vulkan handle
	 * 
	 * @return lyra::vulkan::RAIIContainer::const_handle_type&
	 */
	constexpr const_handle_type& get() const noexcept { return m_handle; }
	/**
	 * @brief get the raw vulkan handle
	 * 
	 * @return lyra::vulkan::RAIIContainer::handle_type&
	 */
	constexpr operator handle_type&() noexcept { return m_handle; }
	/**
	 * @brief get the raw vulkan handle
	 * 
	 * @return lyra::vulkan::RAIIContainer::const_handle_type&
	 */
	constexpr operator const_handle_type&() const noexcept { return m_handle; }

protected:
	handle_type m_handle { };
	owner_type m_owner { };
};

namespace vk {

using NullHandle = decltype(VK_NULL_HANDLE);

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

} // namespace lyra
