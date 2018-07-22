#include "w_render_pch.h"
#include "w_occlusion_query.h"

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			class w_occlusion_query_pimp
			{
			public:
				w_occlusion_query_pimp()
					: _gDevice(nullptr)
				{
				}

				W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const uint32_t& pQuerySize)
				{
					if (!pGDevice) return W_FAILED;

					this->_gDevice = pGDevice;
					this->_passed_samples.resize(pQuerySize, 1);

					VkQueryPoolCreateInfo _query_pool_info = {};
					_query_pool_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
					_query_pool_info.queryType = VK_QUERY_TYPE_OCCLUSION;
					_query_pool_info.queryCount = 2;
					vkCreateQueryPool(pGDevice->vk_device, &_query_pool_info, NULL, &this->_query_pool);

					return W_PASSED;
				}

				uint64_t* wait_for_query_results(_Inout_ size_t& pNumberOfResults)
				{
					pNumberOfResults = this->_passed_samples.size();
					get_query_pool_results(VK_QUERY_RESULT_WAIT_BIT);
					return get_passed_samples_data();
				}

				uint64_t* get_available_query_results(_Inout_ size_t& pNumberOfResults)
				{
					pNumberOfResults = this->_passed_samples.size();
					get_query_pool_results(VK_QUERY_RESULT_WITH_AVAILABILITY_BIT);
					return get_passed_samples_data();
				}

				uint64_t* get_partial_query_results(_Inout_ size_t& pNumberOfResults)
				{
					pNumberOfResults = this->_passed_samples.size();
					get_query_pool_results(VK_QUERY_RESULT_PARTIAL_BIT);
					return get_passed_samples_data();
				}

				void reset(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t& pFirstQuery)
				{
					vkCmdResetQueryPool(
						pCommandBuffer.handle,
						this->_query_pool,
						pFirstQuery,
						static_cast<uint32_t>(this->_passed_samples.size()));
				}

				void begin_query(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t& pQueryIndex)
				{
					vkCmdBeginQuery(pCommandBuffer.handle, this->_query_pool, pQueryIndex, 0);
				}

				void end_query(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t& pQueryIndex)
				{
					vkCmdEndQuery(pCommandBuffer.handle, this->_query_pool, pQueryIndex);
				}

				ULONG release()
				{
					this->_passed_samples.clear();
					this->_gDevice = nullptr;

					return 0;
				}

#pragma region Getters

				uint64_t*	get_passed_samples_data()
				{
					return this->_passed_samples.size() ? &this->_passed_samples[0] : nullptr;
				}
				
#pragma endregion

			private:
				void get_query_pool_results(_In_ const VkQueryResultFlagBits& pQueryResultFlagBits)
				{
					// We use vkGetQueryResults to copy the results into a host visible buffer
					vkGetQueryPoolResults(
						this->_gDevice->vk_device,
						this->_query_pool,
						0,
						static_cast<uint32_t>(this->_passed_samples.size()),
						sizeof(uint64_t) * this->_passed_samples.size(),
						this->_passed_samples.data(),
						sizeof(uint64_t),
						VK_QUERY_RESULT_64_BIT | pQueryResultFlagBits);
				}

				VkQueryPool								_query_pool;
				std::shared_ptr<w_graphics_device>		_gDevice;
				std::vector<uint64_t>					_passed_samples;
			};
		}
	}
}

using namespace wolf::render::vulkan;

w_occlusion_query::w_occlusion_query() : _pimp(new w_occlusion_query_pimp())
{
	_super::set_class_name("w_occlusion_query");
}

w_occlusion_query::~w_occlusion_query()
{
	release();
}

W_RESULT w_occlusion_query::initialize(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const uint32_t& pQuerySize)
{
	return this->_pimp ? this->_pimp->initialize(pGDevice, pQuerySize) : W_FAILED;
}


uint64_t* w_occlusion_query::wait_for_query_results(_Inout_ size_t& pNumberOfResults)
{
	if (this->_pimp) return this->_pimp->wait_for_query_results(pNumberOfResults);
	pNumberOfResults = 0;
	return nullptr;
}

uint64_t* w_occlusion_query::get_available_query_results(_Inout_ size_t& pNumberOfResults)
{
	if (this->_pimp) return this->_pimp->get_available_query_results(pNumberOfResults);
	pNumberOfResults = 0;
	return nullptr;
}

uint64_t* w_occlusion_query::get_partial_query_results(_Inout_ size_t& pNumberOfResults)
{
	if (this->_pimp) return this->_pimp->get_partial_query_results(pNumberOfResults);
	pNumberOfResults = 0;
	return nullptr;
}

void w_occlusion_query::reset(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t& pFirstQuery)
{
	if (!this->_pimp) return;
	this->_pimp->reset(pCommandBuffer, pFirstQuery);
}

void w_occlusion_query::begin_query(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t& pQueryIndex)
{
	if (!this->_pimp) return;
	this->_pimp->begin_query(pCommandBuffer, pQueryIndex);
}

void w_occlusion_query::end_query(_In_ const w_command_buffer& pCommandBuffer, _In_ const uint32_t& pQueryIndex)
{
	if (!this->_pimp) return;
	this->_pimp->end_query(pCommandBuffer, pQueryIndex);
}

ULONG w_occlusion_query::release()
{
    if (_super::get_is_released()) return 1;
    
	//release the private implementation
	SAFE_RELEASE(this->_pimp);

	return _super::release();
}
