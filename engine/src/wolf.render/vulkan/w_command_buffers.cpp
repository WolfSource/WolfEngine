#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include "w_command_buffers.h"
#include <w_convert.h>

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			W_RESULT w_command_buffer::begin(_In_ const uint32_t pFlags)
			{
				if (this->began) return W_PASSED;
				
				//prepare data for recording command buffers
				const VkCommandBufferBeginInfo _command_buffer_begin_info =
				{
					VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,        // Type;
					nullptr,                                            // Next
					(VkCommandBufferUsageFlags)pFlags,					// Flags
					nullptr,
				};

				auto _hr = vkBeginCommandBuffer(this->handle, &_command_buffer_begin_info);
				if (_hr != VK_SUCCESS)
				{
					logger.error("begining command buffer. trace info: w_command_buffer::begin");
					return W_FAILED;
				}

				this->began = true;
				return W_PASSED;
			}
			
			W_RESULT w_command_buffer::begin_secondary(
				_In_ const w_render_pass_handle& pRenderPassHandle,
				_In_ const w_frame_buffer_handle& pFrameBufferHandle,
				_In_ const uint32_t pFlags)
			{
				if (this->began) return W_FAILED;
				
				VkCommandBufferInheritanceInfo _inheritance_info = {};
				_inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
				_inheritance_info.renderPass = pRenderPassHandle.handle;
				_inheritance_info.framebuffer = pFrameBufferHandle.handle;

				VkCommandBufferBeginInfo _sec_cmd_buffer_begin_info{};
				_sec_cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				_sec_cmd_buffer_begin_info.flags = (VkCommandBufferUsageFlags)pFlags;
				_sec_cmd_buffer_begin_info.pInheritanceInfo = &_inheritance_info;

				auto _hr = vkBeginCommandBuffer(this->handle, &_sec_cmd_buffer_begin_info);
				if (_hr != VK_SUCCESS)
				{
					logger.error("begining command buffer. trace info: w_command_buffer::begin_secondary");
					return W_FAILED;
				}
				this->began = true;
				return W_PASSED;
			}
			
			W_RESULT w_command_buffer::end()
			{
				if (!this->began)
				{
					logger.error("command buffer hasn't begun. trace info: w_command_buffer::begin");
					return W_FAILED;
				}

				auto _hr = vkEndCommandBuffer(this->handle);
				if (_hr != VK_SUCCESS)
				{
					logger.error("ending command buffer. trace info: w_command_buffer::end");
					return W_FAILED;
				}
				this->began = false;
				return W_PASSED;
			}

			W_RESULT w_command_buffer::flush(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
			{
				if (end() == W_FAILED)
				{
					logger.error("ending command buffer. trace info: w_command_buffer::flush");
					return W_FAILED;
				}

				VkSubmitInfo _submit_info = {};
				_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				_submit_info.commandBufferCount = 1;
				_submit_info.pCommandBuffers = &this->handle;

				// Create fence to ensure that the command buffer has finished executing
				VkFenceCreateInfo _fence_create_info = {};
				_fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				_fence_create_info.flags = 0;

				VkFence _fence;
				auto _hr = vkCreateFence(
					pGDevice->vk_device,
					&_fence_create_info,
					nullptr,
					&_fence);
				if (_hr)
				{
					logger.error("creating fence for flushing command buffer. trace info: w_command_buffer::flush");
					return W_FAILED;
				}

				// Submit to the present queue
				_hr = vkQueueSubmit(pGDevice->vk_present_queue.queue,
					1,
					&_submit_info,
					_fence);
				if (_hr)
				{
					logger.error("submiting to present queue. trace info: w_command_buffer::flush");
					return W_FAILED;
				}

				// Wait for the fence to signal that command buffer has finished executing
				_hr = vkWaitForFences(pGDevice->vk_device,
					1,
					&_fence,
					VK_TRUE,
					DEFAULT_FENCE_TIMEOUT);
				if (_hr)
				{
					logger.error("wait for fence. trace info: w_command_buffer::flush");
				}
				vkDestroyFence(pGDevice->vk_device, _fence, nullptr);

				return W_PASSED;
			}

			W_RESULT w_command_buffer::execute_secondary_commands(
				_In_ const std::vector<w_command_buffer*>& pSecondaryCommandBuffers)
			{
				auto _size = pSecondaryCommandBuffers.size();
				if (!_size) 
				{
					logger.error("size of secondary command buffers is zero. trace info: w_command_buffer::execute_secondary_commands");
					return W_FAILED;
				}
				std::vector<VkCommandBuffer> _handles(_size);
				for (size_t i = 0; i < _size; ++i)
				{
					auto _cmd = pSecondaryCommandBuffers[i];
					if (!_cmd || !_cmd->handle) continue;
					_handles[i] = _cmd->handle;
				}
				vkCmdExecuteCommands(
					this->handle,
					pSecondaryCommandBuffers.size(),
					_handles.data());
				_handles.clear();

				return W_PASSED;
			}

			class w_command_buffer_pimp
			{
			public:
				w_command_buffer_pimp() :
					_name("w_command_buffers"),
					_command_pool(0)
				{
				}

				W_RESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const size_t& pCount,
					_In_ const w_command_buffer_level& pLevel,
					_In_ const bool& pCreateCommandPool,
					_In_ const w_queue* pCommandPoolQueue)
				{
					const std::string _trace_info = this->_name + "::load";
					
					this->_counts = pCount;
					if (!this->_counts)
					{
						logger.error("invalid count for command buffers");
						return W_FAILED;
					}

					this->_gDevice = pGDevice;

					if (pCreateCommandPool)
					{
						VkCommandPoolCreateInfo _command_pool_info = {};
						_command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
						_command_pool_info.queueFamilyIndex = pCommandPoolQueue ? pCommandPoolQueue->index : pGDevice->vk_graphics_queue.index;
						_command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

						auto _hr = vkCreateCommandPool(
							pGDevice->vk_device,
							&_command_pool_info,
							nullptr,
							&this->_command_pool);
						if (_hr)
						{
							V(W_FAILED,
								w_log_type::W_ERROR,
								"creating vulkan command pool for graphics device: {}. trace info: {}",
								this->_gDevice->get_info(),
								_trace_info);

							return W_FAILED;
						}
					}
					else
					{
						this->_command_pool = this->_gDevice->vk_command_allocator_pool;
					}
					
					//if we have an existing command buffers
					auto _size = this->_commands.size();
					if (_size)
					{
						std::vector<VkCommandBuffer> _cmds(_size);
						for (size_t i = 0; i < _size; ++i)
						{
							_cmds[i] = std::move(_commands[i].handle);
						}
						vkFreeCommandBuffers(this->_gDevice->vk_device,
							this->_command_pool,
							static_cast<uint32_t>(_commands.size()),
							_cmds.data());
						_cmds.clear();
						this->_commands.clear();
					}


					//create the command buffer from the command pool
					VkCommandBufferAllocateInfo _command_buffer_info = {};
					_command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
					_command_buffer_info.pNext = nullptr;
					_command_buffer_info.commandPool = this->_command_pool;
					_command_buffer_info.level = (VkCommandBufferLevel)pLevel;
					_command_buffer_info.commandBufferCount = static_cast<uint32_t>(this->_counts);


					//resize them
					this->_commands.resize(this->_counts);
					std::vector<VkCommandBuffer> _cmds(this->_counts);
					auto _hr = vkAllocateCommandBuffers(this->_gDevice->vk_device,
						&_command_buffer_info,
						_cmds.data());
					if (_hr)
					{
						this->_commands.clear();
						_cmds.clear();

						V(W_FAILED,
							w_log_type::W_ERROR,
							"creating vulkan command buffers for graphics device: {}. trace info: {}",
							this->_gDevice->get_info(),
							_trace_info);

						return W_FAILED;
					}

					for (size_t i = 0; i < this->_counts; ++i)
					{
						this->_commands[i].handle = std::move(_cmds[i]);
					}
					_cmds.clear();
					return W_PASSED;
				}

				W_RESULT begin(_In_ const size_t& pCommandBufferIndex, _In_ const uint32_t pFlags)
				{
					if (pCommandBufferIndex >= this->_commands.size())
					{
						logger.error("invalid index for command buffers");
						return W_FAILED;
					}

					const std::string _trace_info = this->_name + "::begin";

					auto _hr = this->_commands.at(pCommandBufferIndex).begin(pFlags);
					if (_hr != VK_SUCCESS)
					{
						V(W_FAILED,
							w_log_type::W_ERROR,
							"begining command buffer #{} for graphics device: {}. trace info: {}",
							pCommandBufferIndex,
							this->_gDevice->get_info(),
							_trace_info);
						return W_FAILED;
					}

					return W_PASSED;
				}

				W_RESULT begin_secondary(
					_In_ const size_t& pCommandBufferIndex,
					_In_ const w_render_pass_handle& pRenderPassHandle,
					_In_ const w_frame_buffer_handle& pFrameBufferHandle,
					_In_ const uint32_t pFlags)
				{
					if (pCommandBufferIndex >= this->_commands.size())
					{
						logger.error("invalid index for command buffers");
						return W_FAILED;
					}

					const std::string _trace_info = this->_name + "::begin_secondary";

					auto _hr = this->_commands.at(pCommandBufferIndex).begin_secondary(
						pRenderPassHandle,
						pFrameBufferHandle,
						pFlags);
					if (_hr != VK_SUCCESS)
					{
						V(W_FAILED,
							w_log_type::W_ERROR,
							"begining command buffer #{} for graphics device: {}. trace info: {}",
							pCommandBufferIndex,
							this->_gDevice->get_info(),
							_trace_info);
						return W_FAILED;
					}
					return W_PASSED;
				}

				W_RESULT end(_In_ const size_t& pCommandBufferIndex)
				{
					if (pCommandBufferIndex >= this->_commands.size())
					{
						logger.error("invalid index for command buffers");
						return W_FAILED;
					}
					const std::string _trace_info = this->_name + "::end";

					auto _hr = this->_commands.at(pCommandBufferIndex).end();
					if (_hr != VK_SUCCESS)
					{
						V(W_FAILED,
							w_log_type::W_ERROR,
							"ending command buffer #{} for graphics device: {}. trace info: {}:",
							pCommandBufferIndex,
							this->_gDevice->get_info(),
							_trace_info);
					}
					return W_PASSED;
				}

				W_RESULT flush(_In_ const size_t& pCommandBufferIndex)
				{
					if (pCommandBufferIndex >= this->_commands.size())
					{
						logger.error("invalid index for command buffers");
						return W_FAILED;
					}
					const std::string _trace_info = this->_name + "::end";

					auto _hr = this->_commands.at(pCommandBufferIndex).flush(this->_gDevice);
					if (_hr == W_FAILED)
					{
						V(W_FAILED,
							w_log_type::W_ERROR,
							"flushing command buffer #{} for graphics device: {}. trace info: {}",
							pCommandBufferIndex,
							this->_gDevice->get_info(),
							_trace_info);

						return W_FAILED;
					}
					return W_PASSED;
				}

				W_RESULT flush_all()
				{
					W_RESULT _result = W_PASSED;
					for (size_t i = 0; i < this->_commands.size(); ++i)
					{
						if (flush(i) == W_FAILED)
						{
							_result = W_FAILED;
							break;
						}
					}
					return _result;
				}

				W_RESULT execute_secondary_commands(
					_In_ const size_t& pCommandBufferIndex,
					_In_ const std::vector<w_command_buffer*>& pSecondaryCommandBuffers)
				{
					if (pCommandBufferIndex >= this->_commands.size())
					{
						logger.error("invalid index for command buffers");
						return W_FAILED;
					}
					return this->_commands.at(pCommandBufferIndex).execute_secondary_commands(pSecondaryCommandBuffers);
				}

				ULONG release()
				{
					if (this->_gDevice)
					{
						std::vector<VkCommandBuffer> _cmds(this->_commands.size());
						for (size_t i = 0; i < this->_commands.size(); ++i)
						{
							_cmds[i] = std::move(this->_commands[i].handle);
						}
						vkFreeCommandBuffers(this->_gDevice->vk_device,
							this->_command_pool,
							static_cast<uint32_t>(_cmds.size()),
							_cmds.data());
						_cmds.clear();
					}

					this->_commands.clear();
					this->_counts = 0;
					this->_gDevice = nullptr;

					return 0;
				}

#pragma region Getters

				const w_command_buffer* get_commands() const
				{
					return this->_commands.data();
				}

				const size_t get_commands_size() const
				{
					return this->_commands.size();
				}

#pragma endregion 

			private:
				std::string                                         _name;
				std::shared_ptr<w_graphics_device>                  _gDevice;

				std::vector<w_command_buffer>                       _commands;
				VkCommandPool                                       _command_pool;

				size_t                                              _counts;
			};
		}
	}
}

using namespace wolf::render::vulkan;

w_command_buffers::w_command_buffers() : _pimp(new w_command_buffer_pimp())
{
	_super::set_class_name("w_command_buffer");
}

w_command_buffers::~w_command_buffers()
{
	release();
}

W_RESULT w_command_buffers::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const size_t& pCount,
	_In_ const w_command_buffer_level& pLevel,
	_In_ const bool& pCreateCommandPool,
	_In_ const w_queue* pCommandPoolQueue)
{
	if (!this->_pimp) return W_FAILED;

	return this->_pimp->load(pGDevice, pCount, pLevel, pCreateCommandPool, pCommandPoolQueue);
}

W_RESULT w_command_buffers::begin(_In_ const size_t& pCommandBufferIndex, _In_ const uint32_t pFlags)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->begin(pCommandBufferIndex, pFlags);
}

W_RESULT w_command_buffers::begin_secondary(
	_In_ const size_t& pCommandBufferIndex,
	_In_ const w_render_pass_handle& pRenderPassHandle,
	_In_ const w_frame_buffer_handle& pFrameBufferHandle,
	_In_ const uint32_t pFlags)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->begin_secondary(pCommandBufferIndex, pRenderPassHandle, pFrameBufferHandle, pFlags);
}

W_RESULT w_command_buffers::end(_In_ const size_t& pCommandBufferIndex)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->end(pCommandBufferIndex);
}

W_RESULT w_command_buffers::flush(_In_ const size_t& pCommandBufferIndex)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->flush(pCommandBufferIndex);
}

W_RESULT w_command_buffers::flush_all()
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->flush_all();
}

W_RESULT w_command_buffers::execute_secondary_commands(
	_In_ const size_t& pCommandBufferIndex,
	_In_ const std::vector<w_command_buffer*>& pSecondaryCommandBuffers)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->execute_secondary_commands(pCommandBufferIndex, pSecondaryCommandBuffers);
}

ULONG w_command_buffers::release()
{
	if (_super::get_is_released()) return 0;

	SAFE_RELEASE(this->_pimp);

	return _super::release();
}

#pragma region Getters

const w_command_buffer* w_command_buffers::get_commands() const
{
	if (!_pimp) return 0;

	return this->_pimp->get_commands();
}

const w_command_buffer w_command_buffers::get_command_at(_In_ const size_t& pIndex) const
{
	if (!_pimp) return w_command_buffer();

	auto _size = this->_pimp->get_commands_size();
	if (pIndex >= _size) return w_command_buffer();

	auto _cmds = this->_pimp->get_commands();
	return _cmds[pIndex];
}

const size_t w_command_buffers::get_commands_size() const
{
	if (!_pimp) return 0;

	return this->_pimp->get_commands_size();
}

#pragma endregion
