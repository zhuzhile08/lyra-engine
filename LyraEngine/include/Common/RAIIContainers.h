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

using NullHandle = std::nullptr_t;
template <class Ty> concept RAIIContainerType = 
	std::is_copy_constructible_v<Ty> && 
	std::is_copy_assignable_v<Ty> && 
	std::is_move_constructible_v<Ty> &&
	std::is_move_assignable_v<Ty>;

/**
 * @brief A RAII container for making the usage of vulkan handles more secure
 */
template <RAIIContainerType Ty, RAIIContainerType OTy> class RAIIContainer {
public:
	using handle_type = Ty;
	using const_handle = const handle_type;
	using handle_reference = Ty&;
	using const_handle_reference = const_handle&;
	using movable_handle = Ty&&;
	
	using owner_type = OTy;
	using const_owner = const owner_type;
	using owner_reference = owner_type&;
	using const_owner_reference = const_owner&;
	using movable_owner = owner_type&&;

	using container = RAIIContainer;
	using movable_container = container&&;

	constexpr RAIIContainer() = default;
	constexpr RAIIContainer(const_handle_reference handle, const_owner_reference owner) : m_handle(handle), m_owner(owner) { }
	constexpr RAIIContainer(movable_handle handle, movable_owner owner) : m_handle(std::move(handle)), m_owner(std::move(owner)) { }
	constexpr RAIIContainer(movable_container container) : 
		m_handle(std::exchange(container.m_handle, handle_type { } )), 
		m_owner(std::exchange(container.m_owner, owner_type { } )) { }
		
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
			} else if constexpr (std::same_as<handle_type, VkCommandBuffer>) {
				vassert(vkAllocateCommandBuffers(this->m_owner, &createInfo, &this->m_handle), "create command buffer");
			} else if constexpr (std::same_as<handle_type, VkDescriptorSetLayout>) {
				vassert(vkCreateDescriptorSetLayout(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor set layout");
			} else if constexpr (std::same_as<handle_type, VkDescriptorPool>) {
				vassert(vkCreateDescriptorPool(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor pool");
			} else if constexpr (std::same_as<handle_type, VkDescriptorSet>) {
				vassert(vkAllocateDescriptorSets(this->m_owner, &createInfo, &this->m_handle), "allocate descriptor sets");
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
			}  else if constexpr (std::same_as<handle_type, VkPipelineCache>) {
				vassert(vkCreatePipelineCache(&createInfo, nullptr, &this->m_handle), "create instance");
			} else if constexpr (std::same_as<handle_type, VkSwapchainKHR>) {
				vassert(vkCreateSwapchainKHR(this->m_owner, &createInfo, nullptr, &this->m_handle), "create instance");
			} else if constexpr (std::same_as<handle_type, VmaAllocator>) {
				vassert(vmaCreateAllocator(&createInfo, &this->m_handle), "create memory allocator");
			} else if constexpr (std::same_as<handle_type, VmaPool>) {
				vassert(vmaCreatePool(this->m_owner, &createInfo, &this->m_handle), "create memory pool");
			} else if constexpr (std::same_as<handle_type, VkDevice>) {
				vassert(vkCreateDevice(this->m_owner, &createInfo, nullptr, &this->m_handle), "create instance");
			} else if constexpr (std::same_as<handle_type, VkInstance>) {
				vassert(vkCreateInstance(&createInfo, nullptr, &this->m_handle), "create instance");
			}
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is a buffer
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
			vassert(vmaCreateBuffer(allocator, &createInfo, &allocCreateInfo, &m_handle, &allocation.get(), &allocInfo), "create buffer and/or its memory");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is an aligned buffer
	 */
	constexpr RAIIContainer(
		owner_type owner, 
		VmaAllocator allocator, 
		const VkBufferCreateInfo& createInfo, 
		const VmaAllocationCreateInfo& allocCreateInfo, 
		VkDeviceSize minAlignment,
		RAIIContainer<VmaAllocation, VmaAllocator>& allocation, 
		VmaAllocationInfo allocInfo = VmaAllocationInfo { } 
	) requires(std::is_same_v<handle_type, VkBuffer> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			vassert(vmaCreateBufferWithAlignment(allocator, &createInfo, &allocCreateInfo, minAlignment, &m_handle, &allocation.get(), &allocInfo), "create aligned buffer and/or its memory");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is an aliasing buffer
	 */
	constexpr RAIIContainer(
		owner_type owner, 
		VmaAllocator allocator, 
		const VkBufferCreateInfo& createInfo,
		RAIIContainer<VmaAllocation, VmaAllocator>& allocation
	) requires(std::is_same_v<handle_type, VkBuffer> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			vassert(vmaCreateAliasingBuffer(allocator, &allocation.get(), &createInfo, &m_handle), "create aliasing buffer and/or its memory");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is an image
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
			vassert(vmaCreateImage(allocator, &createInfo, &allocCreateInfo, &m_handle, &allocation.get(), &allocInfo), "create image and/or its memory");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is an aliasing image
	 */
	constexpr RAIIContainer(
		owner_type owner, 
		VmaAllocator allocator, 
		const VkImageCreateInfo& createInfo,
		RAIIContainer<VmaAllocation, VmaAllocator>& allocation
	) requires(std::is_same_v<handle_type, VkImage> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			vassert(vmaCreateAliasingImage(allocator, &allocation.get(), &createInfo, &m_handle), "create aliasing image and/or its memory");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the handle is a graphics pipeline
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
	 * @brief construct a new RAIIContainer if the internal handle is a queue
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
	 * @brief construct a new RAIIContainer if the internal handle is a surface
	 */
	constexpr RAIIContainer(owner_type owner, SDL_Window* window) 
		requires(std::is_same_v<handle_type, VkSurfaceKHR> && std::is_same_v<owner_type, VkInstance>)
		 : m_owner(owner) {
		if (!m_handle) {
			SDL_Vulkan_CreateSurface(window, this->m_owner, &this->m_handle);
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the internal handle is a SDL_Window
	 */
	constexpr RAIIContainer(
		std::string_view title, 
		const int& x, 
		const int& y, 
		const int& w, 
		const int& h, 
		const uint32_t& flags
	) requires(std::is_same_v<handle_type, SDL_Window*>) {
		if (!m_handle) {
			m_handle = SDL_CreateWindow(title.data(), x, y, w, h, flags);
		}
	}

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
				} else if constexpr (std::same_as<handle_type, VkPipelineCache>) {
					vkDestroyPipelineCache(m_owner, m_handle, nullptr);
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
				} else if constexpr (std::same_as<handle_type, VmaPool>) {
					vmaDestroyPool(m_owner, m_handle);
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
			} else if constexpr (std::same_as<handle_type, SDL_Window*>) {
				SDL_DestroyWindow(m_handle);
			} // some objects don't need to be destroyed explicitly, so they aren't here, even though they use this RAII container
		}
	}
	virtual ~RAIIContainer() {
		destroy();
	}

	constexpr RAIIContainer& operator=(movable_handle handle) {
		if (handle != this->m_handle) {
			m_handle = std::exchange(handle, handle_type { } );
		}
		return *this;
	}
	constexpr RAIIContainer& operator=(movable_container container) {
		if (&container != this) {
			m_handle = std::exchange(container.m_handle, handle_type { } );
			m_owner = std::exchange(container.m_owner, owner_type { } );
		}
		return *this;
	}

	constexpr handle_type& get() noexcept { return m_handle; }
	constexpr const_handle& get() const noexcept { return m_handle; }
	constexpr operator handle_type&() noexcept { return m_handle; }
	constexpr operator const_handle&() const noexcept { return m_handle; }

protected:
	handle_type m_handle { };
	owner_type m_owner { };
};

namespace sdl {

using Window = RAIIContainer<SDL_Window*, NullHandle>; /// @todo move this to the sdl window file

}

} // namespace lyra
