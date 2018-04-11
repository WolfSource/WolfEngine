#include "w_render_pch.h"
#include "w_masked_occlusion_culling.h"
#include "masked_occlusion_culling/CullingThreadpool.h"
#include <w_thread.h>
#include <w_point.h>

namespace wolf
{
	namespace framework
	{
		class w_masked_occlusion_culling_pimp
		{
		public:
			w_masked_occlusion_culling_pimp() :
				_moc(nullptr),
				_moc_thread_pool(nullptr),
				_multi_threaded(false),
				_enable_debugging(false),
				_moc_per_pixel_zBuffer(nullptr),
				_moc_tone_map_depth_image(nullptr)
			{
			}

			W_RESULT initialize(_In_ uint32_t& pNumberOfWorkerThreads)
			{
				//create masked occlusion culling instnace
				this->_moc = MaskedOcclusionCulling::Create();
				if (!this->_moc) return W_FAILED;

				MaskedOcclusionCulling::Implementation _implementation = _moc->GetImplementation();
				switch (_implementation)
				{
				case MaskedOcclusionCulling::SSE2:
					logger.write("Using SSE2 implementation of MaskedOcclusionCulling");
					break;
				case MaskedOcclusionCulling::SSE41:
					logger.write("Using SSE41 implementation of MaskedOcclusionCulling");
					break;
				case MaskedOcclusionCulling::AVX2:
					logger.write("Using AVX2 implementation of MaskedOcclusionCulling");
					break;
				case MaskedOcclusionCulling::AVX512:
					logger.write("Using AVX512 implementation of MaskedOcclusionCulling");
					break;
				}

				set_number_of_worker_threads(pNumberOfWorkerThreads);
				suspend_threads();

				return W_PASSED;
			}

			W_RESULT suspend_threads()
			{
				if (!this->_moc_thread_pool) return W_FAILED;
				this->_moc_thread_pool->SuspendThreads();
				return W_PASSED;
			}

			W_RESULT wake_threads()
			{
				if (!this->_moc_thread_pool) return W_FAILED;
				this->_moc_thread_pool->WakeThreads();
				return W_PASSED;
			}

			W_RESULT clear_buffer()
			{
				if (this->_multi_threaded)
				{
					if (!this->_moc_thread_pool) return W_FAILED;
					this->_moc_thread_pool->ClearBuffer();
					return W_PASSED;
				}
				else if (this->_moc)
				{
					this->_moc->ClearBuffer();
					return W_PASSED;
				}

				return W_FAILED;
			}

			W_RESULT render_triangles_async(
				_In_ const float* pVertices,
				_In_ const unsigned int* pTriangles,
				_In_ int pNumberOfTriangles,
				_In_ MaskedOcclusionCulling::BackfaceWinding pBackFaceWinding,
				_In_ MaskedOcclusionCulling::ClipPlanes pClipPlaneMask)
			{
				if (!this->_multi_threaded || !this->_moc_thread_pool) return W_FAILED;
				this->_moc_thread_pool->RenderTriangles(pVertices, pTriangles, pNumberOfTriangles, pBackFaceWinding, pClipPlaneMask);
				return W_PASSED;
			}

			W_RESULT render_triangles(
				_In_ const float* pVertices,
				_In_ const unsigned int* pTriangles,
				_In_ int pNumberOfTriangles,
				_In_ const float* pModelToClipMatrix,
				_In_ MaskedOcclusionCulling::BackfaceWinding pBackFaceWinding,
				_In_ MaskedOcclusionCulling::ClipPlanes pClipPlaneMask)
			{
				if (!this->_moc) return W_FAILED;
				this->_moc->RenderTriangles(pVertices, pTriangles, pNumberOfTriangles, pModelToClipMatrix, pBackFaceWinding, pClipPlaneMask);
				return W_PASSED;
			}

			W_RESULT flush()
			{
				if (!this->_multi_threaded || !this->_moc_thread_pool) return W_FAILED;
				this->_moc_thread_pool->Flush();
				return W_PASSED;
			}

			void release()
			{
				if (this->_moc_thread_pool)
				{
					this->_moc_thread_pool->SuspendThreads();
					SAFE_DELETE(this->_moc_thread_pool);
				}

				if (this->_moc)
				{
					MaskedOcclusionCulling::Destroy(this->_moc);
					this->_moc = nullptr;
				}

				if (this->_moc_per_pixel_zBuffer)
				{
					free(this->_moc_per_pixel_zBuffer);
				}

				if (this->_moc_tone_map_depth_image)
				{
					free(this->_moc_tone_map_depth_image);
				}
			}

#pragma region Getters

			uint8_t* get_debug_frame(_In_ bool pFlipY)
			{
				if (!this->_enable_debugging || !this->_moc_per_pixel_zBuffer || !this->_moc_tone_map_depth_image) return nullptr;

				if (this->_moc_thread_pool)
				{
					//Compute a per pixel depth buffer from the hierarchical depth buffer, used for visualization.
					this->_moc_thread_pool->ComputePixelDepthBuffer(this->_moc_per_pixel_zBuffer, pFlipY);
				}
				else if (this->_moc)
				{
					this->_moc->ComputePixelDepthBuffer(this->_moc_per_pixel_zBuffer, pFlipY);
				}
				else
				{
					return nullptr;
				}

				//Tonemap the depth image
				_tone_map_depth(this->_moc_per_pixel_zBuffer, this->_moc_tone_map_depth_image, this->_debug_screen_size.x, this->_debug_screen_size.y);

				return &this->_moc_tone_map_depth_image[0];
			}

#pragma endregion

#pragma region Setters

			void set_number_of_worker_threads(_In_ uint32_t& pNumberOfWorkerThreads)
			{
				auto _thread_pool_size = wolf::system::w_thread::get_number_of_hardware_thread_contexts();
				pNumberOfWorkerThreads %= _thread_pool_size;
				if (pNumberOfWorkerThreads == 0)
				{
					pNumberOfWorkerThreads = 1;
				}

				if (pNumberOfWorkerThreads > 1)
				{
					this->_multi_threaded = true;
					auto _bin_size = _thread_pool_size / 2;

					if (this->_moc_thread_pool)
					{
						this->_moc_thread_pool->SuspendThreads();
						delete this->_moc_thread_pool;
					}

					this->_moc_thread_pool = new CullingThreadpool(_thread_pool_size, _bin_size, _bin_size);
					this->_moc_thread_pool->SetBuffer(this->_moc);
				}
			}

			void set_multi_threaded(_In_ const bool& pValue)
			{
				this->_multi_threaded = pValue;
			}

			void set_enable_debugging(_In_ const bool& pValue)
			{
				this->_enable_debugging = true;
			}

			W_RESULT set_near_clip(_In_ const float& pValue)
			{
				if (_multi_threaded)
				{
					if (!this->_moc_thread_pool) return W_FAILED;
					this->_moc_thread_pool->SetNearClipPlane(pValue);
				}
				else
				{
					if (!this->_moc) return W_FAILED;
					this->_moc->SetNearClipPlane(pValue);
				}
				return W_PASSED;
			}

			W_RESULT set_resolution(_In_ const float& pWidth, _In_ const float& pHeight)
			{
				if (_multi_threaded)
				{
					if (!this->_moc_thread_pool) return W_FAILED;
					suspend_threads();
					this->_moc_thread_pool->SetResolution(pWidth, pHeight);
					wake_threads();
				}
				else
				{
					if (!this->_moc) return W_FAILED;
					this->_moc->SetResolution(pWidth, pHeight);
				}

				if (!this->_enable_debugging || 
					(this->_debug_screen_size.x == pWidth && this->_debug_screen_size.y == pHeight))
					return W_PASSED;
				
				this->_debug_screen_size.x = pWidth;
				this->_debug_screen_size.y = pHeight;

				auto _size = this->_debug_screen_size.x * this->_debug_screen_size.y;

				if (suspend_threads() == W_PASSED)
				{
					if (this->_moc_per_pixel_zBuffer)
					{
						free(this->_moc_per_pixel_zBuffer);
					}
					this->_moc_per_pixel_zBuffer = (float*)malloc(_size * sizeof(float));

					if (this->_moc_tone_map_depth_image)
					{
						free(this->_moc_tone_map_depth_image);
					}
					this->_moc_tone_map_depth_image = (uint8_t*)malloc(_size * 4 * sizeof(uint8_t));

					return wake_threads();
				}

				return W_FAILED;
			}

			W_RESULT set_matrix(_In_ const float* pModelToClipMatrix)
			{
				if (_multi_threaded && this->_moc_thread_pool)
				{
					this->_moc_thread_pool->SetMatrix(pModelToClipMatrix);
					return W_PASSED;
				}
				return W_FAILED;
			}

#pragma endregion

		private:

			static void _tone_map_depth(
				_In_ float* pDepth, 
				_In_ unsigned char* pImage, 
				_In_ const int& pW, 
				_In_ const int& pH)
			{
				// Find min/max w coordinate (discard cleared pixels)
				float _minW = FLT_MAX, _maxW = 0.0f;
				for (int i = 0; i < pW * pH; ++i)
				{
					if (pDepth[i] > 0.0f)
					{
						_minW = std::min(_minW, pDepth[i]);
						_maxW = std::max(_maxW, pDepth[i]);
					}
				}

				// Tonemap depth values
				for (int i = 0; i < pW * pH; ++i)
				{
					int intensity = 0;
					if (pDepth[i] > 0)
					{
						intensity = (unsigned char)(223.0*(pDepth[i] - _minW) / (_maxW - _minW) + 32.0);
					}

					auto _index = i * 4;
					pImage[_index + 0] = intensity;
					pImage[_index + 1] = intensity;
					pImage[_index + 2] = intensity;
					pImage[_index + 3] = 255;
				}
			}

			MaskedOcclusionCulling* _moc;
			CullingThreadpool*		_moc_thread_pool;
			bool					_multi_threaded;
			bool                    _enable_debugging;
			w_point_t				_debug_screen_size;
			float*                  _moc_per_pixel_zBuffer;
			uint8_t*                _moc_tone_map_depth_image;
		};
	}
}

using namespace wolf::framework;

w_masked_occlusion_culling::w_masked_occlusion_culling() : _pimp(new w_masked_occlusion_culling_pimp())
{
}

w_masked_occlusion_culling::~w_masked_occlusion_culling()
{
	release();
}

W_RESULT w_masked_occlusion_culling::initialize(_In_ uint32_t& pNumberOfWorkerThreads)
{
	return ((this->_pimp) ? this->_pimp->initialize(pNumberOfWorkerThreads) : W_FAILED);
}

W_RESULT w_masked_occlusion_culling::suspend_threads()
{
	return ((this->_pimp) ? this->_pimp->suspend_threads() : W_FAILED);
}

W_RESULT w_masked_occlusion_culling::wake_threads()
{
	return ((this->_pimp) ? this->_pimp->wake_threads() : W_FAILED);
}

W_RESULT w_masked_occlusion_culling::clear_buffer()
{
	return ((this->_pimp) ? this->_pimp->clear_buffer() : W_FAILED);
}

W_RESULT w_masked_occlusion_culling::render_triangles_async(
	_In_ const float* pVertices,
	_In_ const unsigned int* pTriangles,
	_In_ const int& pNumberOfTriangles,
	_In_ const MaskedOcclusionCulling::BackfaceWinding& pBackFaceWinding,
	_In_ const MaskedOcclusionCulling::ClipPlanes& pClipPlaneMask)
{
	return (this->_pimp ? this->_pimp->render_triangles_async(pVertices, pTriangles, pNumberOfTriangles, pBackFaceWinding, pClipPlaneMask) : W_FAILED);
}

W_RESULT w_masked_occlusion_culling::render_triangles(
	_In_ const float* pVertices,
	_In_ const unsigned int* pTriangles,
	_In_ const int& pNumberOfTriangles,
	_In_ const float* pModelToClipMatrix,
	_In_ const MaskedOcclusionCulling::BackfaceWinding& pBackFaceWinding,
	_In_ const MaskedOcclusionCulling::ClipPlanes& pClipPlaneMask)
{
	return (this->_pimp ? this->_pimp->render_triangles(pVertices, pTriangles, pNumberOfTriangles, pModelToClipMatrix, pBackFaceWinding, pClipPlaneMask) : W_FAILED);
}

W_RESULT w_masked_occlusion_culling::flush()
{
	return (this->_pimp ? this->_pimp->flush() : W_FAILED);
}

ULONG w_masked_occlusion_culling::release()
{
	if (this->get_is_released()) return 1;

	SAFE_RELEASE(this->_pimp);

	return _super::release();
}

#pragma region Getters

uint8_t* w_masked_occlusion_culling::get_debug_frame(_In_ bool pFlipY)
{
	if (!this->_pimp) return nullptr;
	return this->_pimp->get_debug_frame(pFlipY);
}

#pragma endregion

#pragma region Setters

void w_masked_occlusion_culling::set_number_of_worker_threads(_In_ uint32_t& pNumberOfWorkerThreads)
{
	if (this->_pimp) this->_pimp->set_number_of_worker_threads(pNumberOfWorkerThreads);
}

void w_masked_occlusion_culling::set_multi_threaded(_In_ const bool& pValue)
{
	if (this->_pimp) this->_pimp->set_multi_threaded(pValue);
}

void w_masked_occlusion_culling::set_enable_debugging(_In_ const bool& pValue)
{
	if (this->_pimp) this->_pimp->set_enable_debugging(pValue);
}

W_RESULT w_masked_occlusion_culling::set_near_clip(_In_ const float& pValue)
{
	return (this->_pimp ? this->_pimp->set_near_clip(pValue) : W_FAILED);
}

W_RESULT w_masked_occlusion_culling::set_resolution(_In_ const float& pWidth, _In_ const float& pHeight)
{
	return (this->_pimp ? this->_pimp->set_resolution(pWidth, pHeight) : W_FAILED);
}

void w_masked_occlusion_culling::set_matrix(_In_ const float* pMatrix)
{
	if (this->_pimp && pMatrix)
	{
		this->_pimp->set_matrix(pMatrix);
	}
}
#pragma endregion

