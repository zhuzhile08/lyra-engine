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

using NullHandle = nullpointer;
template <class Ty> concept RAIIContainerType = 
	std::is_copy_constructible_v<Ty> && 
	std::is_copy_assignable_v<Ty> && 
	std::is_move_constructible_v<Ty> &&
	std::is_move_assignable_v<Ty>;

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

	constexpr RAIIContainer() noexcept = default;
	constexpr RAIIContainer(const_handle_reference handle, const_owner_reference owner) noexcept : m_handle(handle), m_owner(owner) { }
	constexpr RAIIContainer(movable_handle handle, movable_owner owner) noexcept : m_handle(std::move(handle)), m_owner(std::move(owner)) { }
	constexpr RAIIContainer(movable_container container) noexcept : 
		m_handle(std::exchange(container.m_handle, handle_type { } )), 
		m_owner(std::exchange(container.m_owner, owner_type { } )) { }
		
	template <class CI> constexpr RAIIContainer(owner_type owner, const CI& createInfo) : RAIIContainer<Ty, OTy>(Ty { }, owner) { 
		if (!this->m_handle) {
			if constexpr (std::same_as<handle_type, VkFramebuffer>) {
				VULKAN_ASSERT(vkCreateFramebuffer(this->m_owner, &createInfo, nullptr, &this->m_handle), "create framebuffer");
			} else if constexpr (std::same_as<handle_type, VkRenderPass>) {
				VULKAN_ASSERT(vkCreateRenderPass(this->m_owner, &createInfo, nullptr, &this->m_handle), "create render pass");
			} else if constexpr (std::same_as<handle_type, VkSampler>) {
				VULKAN_ASSERT(vkCreateSampler(this->m_owner, &createInfo, nullptr, &this->m_handle), "create image sampler");
			} else if constexpr (std::same_as<handle_type, VkCommandPool>) {
				VULKAN_ASSERT(vkCreateCommandPool(this->m_owner, &createInfo, nullptr, &this->m_handle), "create command pool");
			} else if constexpr (std::same_as<handle_type, VkCommandBuffer>) {
				VULKAN_ASSERT(vkAllocateCommandBuffers(this->m_owner, &createInfo, &this->m_handle), "create command buffer");
			} else if constexpr (std::same_as<handle_type, VkDescriptorSetLayout>) {
				VULKAN_ASSERT(vkCreateDescriptorSetLayout(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor set layout");
			} else if constexpr (std::same_as<handle_type, VkDescriptorPool>) {
				VULKAN_ASSERT(vkCreateDescriptorPool(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor pool");
			} else if constexpr (std::same_as<handle_type, VkDescriptorSet>) {
				VULKAN_ASSERT(vkAllocateDescriptorSets(this->m_owner, &createInfo, &this->m_handle), "allocate descriptor sets");
			} else if constexpr (std::same_as<handle_type, VkDescriptorUpdateTemplate>) {
				VULKAN_ASSERT(vkCreateDescriptorUpdateTemplate(this->m_owner, &createInfo, nullptr, &this->m_handle), "create descriptor update template");
			} else if constexpr (std::same_as<handle_type, VkSemaphore>) {
				VULKAN_ASSERT(vkCreateSemaphore(this->m_owner, &createInfo, nullptr, &this->m_handle), "create semaphore");
			} else if constexpr (std::same_as<handle_type, VkFence>) {
				VULKAN_ASSERT(vkCreateFence(this->m_owner, &createInfo, nullptr, &this->m_handle), "create fence");
			} else if constexpr (std::same_as<handle_type, VkImageView>) {
				VULKAN_ASSERT(vkCreateImageView(this->m_owner, &createInfo, nullptr, &this->m_handle), "create image view");
			} else if constexpr (std::same_as<handle_type, VkPipelineLayout>) {
				VULKAN_ASSERT(vkCreatePipelineLayout(this->m_owner, &createInfo, nullptr, &this->m_handle), "create pipeline layout");
			} else if constexpr (std::same_as<handle_type, VkShaderModule>) {
				VULKAN_ASSERT(vkCreateShaderModule(this->m_owner, &createInfo, nullptr, &this->m_handle), "create shader module");
			}  else if constexpr (std::same_as<handle_type, VkPipelineCache>) {
				VULKAN_ASSERT(vkCreatePipelineCache(&createInfo, nullptr, &this->m_handle), "create pipeline cache");
			} else if constexpr (std::same_as<handle_type, VkSwapchainKHR>) {
				VULKAN_ASSERT(vkCreateSwapchainKHR(this->m_owner, &createInfo, nullptr, &this->m_handle), "create swapchain");
			} else if constexpr (std::same_as<handle_type, VmaAllocator>) {
				VULKAN_ASSERT(vmaCreateAllocator(&createInfo, &this->m_handle), "create memory allocator");
			} else if constexpr (std::same_as<handle_type, VmaPool>) {
				VULKAN_ASSERT(vmaCreatePool(this->m_owner, &createInfo, &this->m_handle), "create memory pool");
			} else if constexpr (std::same_as<handle_type, VkDevice>) {
				VULKAN_ASSERT(vkCreateDevice(this->m_owner, &createInfo, nullptr, &this->m_handle), "create device");
			} else if constexpr (std::same_as<handle_type, VkInstance>) {
				VULKAN_ASSERT(vkCreateInstance(&createInfo, nullptr, &this->m_handle), "create instance");
			} else if constexpr (std::same_as<handle_type, VkDebugUtilsMessengerEXT>) {
				static auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(this->m_owner, "vkCreateDebugUtilsMessengerEXT");
				ASSERT(func, "vkGetInstanceProcAddr() for \"vkCreateDebugUtilsMessengerEXT\" returned a null poiner!");
				VULKAN_ASSERT(func(this->m_owner, &createInfo, nullptr, &this->m_handle), "create debug messenger");
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
			VULKAN_ASSERT(vmaCreateBuffer(allocator, &createInfo, &allocCreateInfo, &m_handle, &allocation.get(), &allocInfo), "create buffer and/or its memory");
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
			VULKAN_ASSERT(vmaCreateBufferWithAlignment(allocator, &createInfo, &allocCreateInfo, minAlignment, &m_handle, &allocation.get(), &allocInfo), "create aligned buffer and/or its memory");
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
			VULKAN_ASSERT(vmaCreateAliasingBuffer(allocator, &allocation.get(), &createInfo, &m_handle), "create aliasing buffer and/or its memory");
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
			VULKAN_ASSERT(vmaCreateImage(allocator, &createInfo, &allocCreateInfo, &m_handle, &allocation.get(), &allocInfo), "create image and/or its memory");
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
			VULKAN_ASSERT(vmaCreateAliasingImage(allocator, &allocation.get(), &createInfo, &m_handle), "create aliasing image and/or its memory");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the handle is a graphics pipeline
	 */
	constexpr RAIIContainer(
		owner_type owner, 
		VkPipelineCache pipelineCache, 
		const VkGraphicsPipelineCreateInfo& createInfo
	) requires(std::is_same_v<handle_type, VkPipeline> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			VULKAN_ASSERT(vkCreateGraphicsPipelines(m_owner, pipelineCache, 1, &createInfo, nullptr, &m_handle), "create graphics pipeline");
		}
	}
	/**
	 * @brief construct a new RAIIContainer if the handle is a compute pipeline
	 */
	constexpr RAIIContainer(
		owner_type owner, 
		VkPipelineCache pipelineCache, 
		const VkComputePipelineCreateInfo& createInfo
	) requires(std::is_same_v<handle_type, VkPipeline> && std::is_same_v<owner_type, VkDevice>) : m_owner(owner) { 
		if (!m_handle) {
			VULKAN_ASSERT(vkCreateComputePipelines(m_owner, pipelineCache, 1, &createInfo, nullptr, &m_handle), "create compute pipeline");
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
				} else if constexpr (std::same_as<handle_type, VkDescriptorUpdateTemplate>) {
					vkDestroyDescriptorUpdateTemplate(m_owner, m_handle, nullptr);
				}  else if constexpr (std::same_as<handle_type, VkSemaphore>) {
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
			} else if constexpr (std::same_as<handle_type, VkDebugUtilsMessengerEXT>) {
				auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(this->m_owner, "vkDestroyDebugUtilsMessengerEXT");
				if (func) func(this->m_owner, this->m_handle, nullptr);
			} else if constexpr (std::same_as<handle_type, SDL_Window*>) {
				SDL_DestroyWindow(m_handle);
			} // some objects don't need to be destroyed explicitly, so they aren't here, even though they use this RAII container
		}
	}
	virtual ~RAIIContainer() {
		destroy();
	}

	constexpr RAIIContainer& operator=(movable_handle handle) noexcept {
		if (handle != this->m_handle) {
			m_handle = std::exchange(handle, handle_type { } );
		}
		return *this;
	}
	constexpr RAIIContainer& operator=(movable_container container) noexcept {
		if (&container != this) {
			m_handle = std::exchange(container.m_handle, handle_type { } );
			m_owner = std::exchange(container.m_owner, owner_type { } );
		}
		return *this;
	}

	constexpr handle_type release() noexcept {
		return std::exchange(m_handle, handle_type { });
	}

	constexpr handle_type& get() noexcept { return m_handle; }
	constexpr const_handle& get() const noexcept { return m_handle; }
	constexpr operator handle_type&() noexcept { return m_handle; }
	constexpr operator const_handle&() const noexcept { return m_handle; }
	template <class DTy> constexpr operator RAIIContainer<handle_type, DTy>() noexcept { return RAIIContainer<handle_type, DTy>(m_handle, DTy()); } // specifically for casting to weak container variant

protected:
	handle_type m_handle { };
	owner_type m_owner { };
};

namespace sdl {

using Window = RAIIContainer<SDL_Window*, NullHandle>; /// @todo move this to the sdl window file

}

} // namespace lyra
