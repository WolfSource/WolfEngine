/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_graphics_device_manager.h
    Description      : The python exporter for w_graphics_device_manager class and structs
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __PY_GRAPHICS_DEVICE_MANAGER_H__
#define __PY_GRAPHICS_DEVICE_MANAGER_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	static void py_graphics_device_manager_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//export w_graphics_device_manager_configs class
		class_<w_graphics_device_manager_configs>("w_graphics_device_manager_configs", init<>())
			.def_readwrite("debug_gpu", &w_graphics_device_manager_configs::debug_gpu, "debug_gpu")
			.def_readwrite("off_screen_mode", &w_graphics_device_manager_configs::off_screen_mode, "off_screen_mode")
			;

		//export w_viewport class
		class_<w_viewport>("w_viewport", init<>())
			.def_readwrite("x", &w_viewport::x, "x")
			.def_readwrite("y", &w_viewport::y, "y")
			.def_readwrite("width", &w_viewport::width, "width")
			.def_readwrite("height", &w_viewport::height, "height")
			.def_readwrite("minDepth", &w_viewport::minDepth, "minDepth")
			.def_readwrite("maxDepth", &w_viewport::maxDepth, "maxDepth")
			;

		//export w_viewport_scissor class
		class_<w_viewport_scissor>("w_viewport_scissor", init<>())
			.add_property("offset", &w_viewport_scissor::py_get_offset, &w_viewport_scissor::py_set_offset, "offset")
			.add_property("extent", &w_viewport_scissor::py_get_extent, &w_viewport_scissor::py_set_extent, "extent")
			;

		//export w_attachment_buffer_desc class
		//TODO: Deeper data needed for desc and ref
		class_<w_attachment_buffer_desc>("w_attachment_buffer_desc", init<>())
			.def_readwrite("desc", &w_attachment_buffer_desc::desc, "attachment desc")
			.def_readwrite("ref", &w_attachment_buffer_desc::ref, "attachment reference")
			.def_readwrite("memory_flag", &w_attachment_buffer_desc::memory_flag, "memory flag bits")
			.def("create_color_desc_buffer", &w_attachment_buffer_desc::create_color_desc_buffer, "create color desc buffer")
			.staticmethod("create_color_desc_buffer")
			.def("create_depth_desc_buffer", &w_attachment_buffer_desc::create_depth_desc_buffer, "create depth desc buffer")
			.staticmethod("create_depth_desc_buffer")
			;

		//export w_attachment_description class
		class_<w_attachment_description, boost::noncopyable>("w_attachment_description", init<>());

		//export w_attachment_reference class
		class_<w_attachment_reference, boost::noncopyable>("w_attachment_reference", init<>());
		
		//export w_push_constant_range class
		class_<w_push_constant_range, boost::noncopyable>("w_push_constant_range", init<>());

		//export w_sampler class
		class_<w_sampler, boost::noncopyable>("w_sampler", init<>());

		//export w_descriptor_buffer_info class
		class_<w_descriptor_buffer_info>("w_descriptor_buffer_info", init<>());

		//export w_descriptor_image_info class
		class_<w_descriptor_image_info>("w_descriptor_image_info", init<>());

		//export w_pipeline_layout_create_info struct
		class_<w_pipeline_layout_create_info, boost::noncopyable>("w_pipeline_layout_create_info", init<>());

		//export w_pipeline_vertex_input_state_create_info struct
		class_<w_pipeline_vertex_input_state_create_info, boost::noncopyable>("w_pipeline_vertex_input_state_create_info", init<>());

		//export w_pipeline_input_assembly_state_create_info struct
		class_<w_pipeline_input_assembly_state_create_info, boost::noncopyable>("w_pipeline_input_assembly_state_create_info", init<>());

		//export w_pipeline_rasterization_state_create_info struct
		class_<w_pipeline_rasterization_state_create_info, boost::noncopyable>("w_pipeline_rasterization_state_create_info", init<>());

		//export w_pipeline_multisample_state_create_info struct
		class_<w_pipeline_multisample_state_create_info, boost::noncopyable>("w_pipeline_multisample_state_create_info", init<>());

		//export w_pipeline_color_blend_attachment_state struct
		class_<w_pipeline_color_blend_attachment_state, boost::noncopyable>("w_pipeline_color_blend_attachment_state", init<>());

		//define w_memory_property_flag_bits enum
		enum_<w_memory_property_flag_bits>("w_memory_property_flag_bits")
			.value("DEVICE_LOCAL_BIT", w_memory_property_flag_bits::DEVICE_LOCAL_BIT)
			.value("HOST_VISIBLE_BIT", w_memory_property_flag_bits::HOST_VISIBLE_BIT)
			.value("HOST_COHERENT_BIT", w_memory_property_flag_bits::HOST_COHERENT_BIT)
			.value("HOST_CACHED_BIT", w_memory_property_flag_bits::HOST_CACHED_BIT)
			.value("LAZILY_ALLOCATED_BIT", w_memory_property_flag_bits::LAZILY_ALLOCATED_BIT)
			.value("FLAG_BITS_MAX_ENUM", w_memory_property_flag_bits::W_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM)
			.export_values()
			;

		//define w_pipeline_stage_flag_bits enum
		enum_<w_pipeline_stage_flag_bits>("w_pipeline_stage_flag_bits")
			.value("TOP_OF_PIPE_BIT", w_pipeline_stage_flag_bits::TOP_OF_PIPE_BIT)
			.value("DRAW_INDIRECT_BIT", w_pipeline_stage_flag_bits::DRAW_INDIRECT_BIT)
			.value("VERTEX_INPUT_BIT", w_pipeline_stage_flag_bits::VERTEX_INPUT_BIT)
			.value("VERTEX_SHADER_BIT", w_pipeline_stage_flag_bits::VERTEX_SHADER_BIT)
			.value("TESSELLATION_CONTROL_SHADER_BIT", w_pipeline_stage_flag_bits::TESSELLATION_CONTROL_SHADER_BIT)
			.value("TESSELLATION_EVALUATION_SHADER_BIT", w_pipeline_stage_flag_bits::TESSELLATION_EVALUATION_SHADER_BIT)
			.value("GEOMETRY_SHADER_BIT", w_pipeline_stage_flag_bits::GEOMETRY_SHADER_BIT)
			.value("FRAGMENT_SHADER_BIT", w_pipeline_stage_flag_bits::FRAGMENT_SHADER_BIT)
			.value("EARLY_FRAGMENT_TESTS_BIT", w_pipeline_stage_flag_bits::EARLY_FRAGMENT_TESTS_BIT)
			.value("LATE_FRAGMENT_TESTS_BIT", w_pipeline_stage_flag_bits::LATE_FRAGMENT_TESTS_BIT)
			.value("COLOR_ATTACHMENT_OUTPUT_BIT", w_pipeline_stage_flag_bits::COLOR_ATTACHMENT_OUTPUT_BIT)
			.value("COMPUTE_SHADER_BIT", w_pipeline_stage_flag_bits::COMPUTE_SHADER_BIT)
			.value("TRANSFER_BIT", w_pipeline_stage_flag_bits::TRANSFER_BIT)
			.value("BOTTOM_OF_PIPE_BIT", w_pipeline_stage_flag_bits::BOTTOM_OF_PIPE_BIT)
			.value("HOST_BIT", w_pipeline_stage_flag_bits::HOST_BIT)
			.value("ALL_GRAPHICS_BIT", w_pipeline_stage_flag_bits::ALL_GRAPHICS_BIT)
			.value("ALL_COMMANDS_BIT", w_pipeline_stage_flag_bits::ALL_COMMANDS_BIT)
			.value("COMMAND_PROCESS_BIT_NVX", w_pipeline_stage_flag_bits::COMMAND_PROCESS_BIT_NVX)
			.value("MAX_ENUM", w_pipeline_stage_flag_bits::W_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM)
			.export_values()
			;

		//define w_dynamic_state enum
		enum_<w_dynamic_state>("w_dynamic_state")
			.value("VIEWPORT", w_dynamic_state::VIEWPORT)
			.value("SCISSOR", w_dynamic_state::SCISSOR)
			.value("LINE_WIDTH", w_dynamic_state::LINE_WIDTH)
			.value("DEPTH_BIAS", w_dynamic_state::DEPTH_BIAS)
			.value("BLEND_CONSTANTS", w_dynamic_state::BLEND_CONSTANTS)
			.value("DEPTH_BOUNDS", w_dynamic_state::DEPTH_BOUNDS)
			.value("STENCIL_COMPARE_MASK", w_dynamic_state::STENCIL_COMPARE_MASK)
			.value("STENCIL_WRITE_MASK", w_dynamic_state::STENCIL_WRITE_MASK)
			.value("STENCIL_REFERENCE", w_dynamic_state::STENCIL_REFERENCE)
			.value("VIEWPORT_W_SCALING_NV", w_dynamic_state::VIEWPORT_W_SCALING_NV)
			.value("DISCARD_RECTANGLE_EXT", w_dynamic_state::DISCARD_RECTANGLE_EXT)
			.value("SAMPLE_LOCATIONS_EXT", w_dynamic_state::SAMPLE_LOCATIONS_EXT)
			.value("BEGIN_RANGE", w_dynamic_state::W_DYNAMIC_STATE_BEGIN_RANGE)
			.value("END_RANGE", w_dynamic_state::W_DYNAMIC_STATE_END_RANGE)
			.value("RANGE_SIZE", w_dynamic_state::W_DYNAMIC_STATE_RANGE_SIZE)
			.value("MAX_ENUM", w_dynamic_state::W_DYNAMIC_STATE_MAX_ENUM)
			.export_values()
			;

		//define w_primitive_topology enum
		enum_<w_primitive_topology>("w_primitive_topology")
			.value("POINT_LIST", w_primitive_topology::POINT_LIST)
			.value("LINE_LIST", w_primitive_topology::LINE_LIST)
			.value("LINE_STRIP", w_primitive_topology::LINE_STRIP)
			.value("TRIANGLE_LIST", w_primitive_topology::TRIANGLE_LIST)
			.value("TRIANGLE_STRIP", w_primitive_topology::TRIANGLE_STRIP)
			.value("TRIANGLE_FAN", w_primitive_topology::TRIANGLE_FAN)
			.value("LINE_LIST_WITH_ADJACENCY", w_primitive_topology::LINE_LIST_WITH_ADJACENCY)
			.value("LINE_STRIP_WITH_ADJACENCY", w_primitive_topology::LINE_STRIP_WITH_ADJACENCY)
			.value("TRIANGLE_LIST_WITH_ADJACENCY", w_primitive_topology::TRIANGLE_LIST_WITH_ADJACENCY)
			.value("TRIANGLE_STRIP_WITH_ADJACENCY", w_primitive_topology::TRIANGLE_STRIP_WITH_ADJACENCY)
			.value("PATCH_LIST", w_primitive_topology::PATCH_LIST)
			.value("BEGIN_RANGE", w_primitive_topology::W_PRIMITIVE_TOPOLOGY_BEGIN_RANGE)
			.value("END_RANGE", w_primitive_topology::W_PRIMITIVE_TOPOLOGY_END_RANGE)
			.value("RANGE_SIZE", w_primitive_topology::W_PRIMITIVE_TOPOLOGY_RANGE_SIZE)
			.value("MAX_ENUM", w_primitive_topology::W_PRIMITIVE_TOPOLOGY_MAX_ENUM)
			.export_values()
			;

		//define w_image_type enum
		enum_<w_image_type>("w_image_type")
			.value("_1D", w_image_type::_1D_TYPE)
			.value("_2D", w_image_type::_2D_TYPE)
			.value("_3D", w_image_type::_3D_TYPE)
			.value("BEGIN_RANGE", w_image_type::W_IMAGE_TYPE_BEGIN_RANGE)
			.value("END_RANGE", w_image_type::W_IMAGE_TYPE_END_RANGE)
			.value("RANGE_SIZE", w_image_type::W_IMAGE_TYPE_RANGE_SIZE)
			.value("MAX_ENUM", w_image_type::W_IMAGE_TYPE_MAX_ENUM)
			.export_values()
			;

		//define w_image_view_type enum
		enum_<w_image_view_type>("w_image_view_type")
			.value("_1D", w_image_view_type::_1D)
			.value("_2D", w_image_view_type::_2D)
			.value("_3D", w_image_view_type::_3D)
			.value("CUBE", w_image_view_type::CUBE)
			.value("_1D_ARRAY", w_image_view_type::_1D_ARRAY)
			.value("_2D_ARRAY", w_image_view_type::_2D_ARRAY)
			.value("CUBE_ARRAY", w_image_view_type::CUBE_ARRAY)
			.value("W_IMAGE_VIEW_TYPE_BEGIN_RANGE", w_image_view_type::W_IMAGE_VIEW_TYPE_BEGIN_RANGE)
			.value("W_IMAGE_VIEW_TYPE_END_RANGE", w_image_view_type::W_IMAGE_VIEW_TYPE_END_RANGE)
			.value("W_IMAGE_VIEW_TYPE_MAX_ENUM", w_image_view_type::W_IMAGE_VIEW_TYPE_MAX_ENUM)
			.value("W_IMAGE_VIEW_TYPE_RANGE_SIZE", w_image_view_type::W_IMAGE_VIEW_TYPE_RANGE_SIZE)
			.export_values()
			;

		//define w_image_usage_flag_bits enum
		enum_<w_image_usage_flag_bits>("w_image_usage_flag_bits")
			.value("TRANSFER_SRC_BIT", w_image_usage_flag_bits::TRANSFER_SRC_BIT)
			.value("TRANSFER_DST_BIT", w_image_usage_flag_bits::TRANSFER_DST_BIT)
			.value("SAMPLED_BIT", w_image_usage_flag_bits::SAMPLED_BIT)
			.value("STORAGE_BIT", w_image_usage_flag_bits::STORAGE_BIT)
			.value("COLOR_ATTACHMENT_BIT", w_image_usage_flag_bits::COLOR_ATTACHMENT_BIT)
			.value("DEPTH_STENCIL_ATTACHMENT_BIT", w_image_usage_flag_bits::DEPTH_STENCIL_ATTACHMENT_BIT)
			.value("TRANSIENT_ATTACHMENT_BIT", w_image_usage_flag_bits::TRANSIENT_ATTACHMENT_BIT)
			.value("INPUT_ATTACHMENT_BIT", w_image_usage_flag_bits::INPUT_ATTACHMENT_BIT)
			.value("MAX_ENUM", w_image_usage_flag_bits::W_IMAGE_USAGE_FLAG_BITS_MAX_ENUM)
			.export_values()
			;

		//define w_format enum
		enum_<w_format>("w_format")
			.value("UNDEFINED", w_format::UNDEFINED)
			.value("R4G4_UNORM_PACK8", w_format::R4G4_UNORM_PACK8)
			.value("R4G4B4A4_UNORM_PACK16", w_format::R4G4B4A4_UNORM_PACK16)
			.value("B4G4R4A4_UNORM_PACK16", w_format::B4G4R4A4_UNORM_PACK16)
			.value("R5G6B5_UNORM_PACK16", w_format::R5G6B5_UNORM_PACK16)
			.value("B5G6R5_UNORM_PACK16", w_format::B5G6R5_UNORM_PACK16)
			.value("R5G5B5A1_UNORM_PACK16", w_format::R5G5B5A1_UNORM_PACK16)
			.value("B5G5R5A1_UNORM_PACK16", w_format::B5G5R5A1_UNORM_PACK16)
			.value("A1R5G5B5_UNORM_PACK16", w_format::A1R5G5B5_UNORM_PACK16)
			.value("R8_UNORM", w_format::R8_UNORM)
			.value("R8_SNORM", w_format::R8_SNORM)
			.value("R8_USCALED", w_format::R8_USCALED)
			.value("R8_SSCALED", w_format::R8_SSCALED)
			.value("R8_UINT", w_format::R8_UINT)
			.value("R8_SINT", w_format::R8_SINT)
			.value("R8_SRGB", w_format::R8_SRGB)
			.value("R8G8_UNORM", w_format::R8G8_UNORM)
			.value("R8G8_SNORM", w_format::R8G8_SNORM)
			.value("R8G8_USCALED", w_format::R8G8_USCALED)
			.value("R8G8_SSCALED", w_format::R8G8_SSCALED)
			.value("R8G8_UINT", w_format::R8G8_UINT)
			.value("R8G8_SINT", w_format::R8G8_SINT)
			.value("R8G8_SRGB", w_format::R8G8_SRGB)
			.value("R8G8B8_UNORM", w_format::R8G8B8_UNORM)
			.value("R8G8B8_SNORM", w_format::R8G8B8_SNORM)
			.value("R8G8B8_USCALED", w_format::R8G8B8_USCALED)
			.value("R8G8B8_SSCALED", w_format::R8G8B8_SSCALED)
			.value("R8G8B8_UINT", w_format::R8G8B8_UINT)
			.value("R8G8B8_SINT", w_format::R8G8B8_SINT)
			.value("R8G8B8_SRGB", w_format::R8G8B8_SRGB)
			.value("B8G8R8_UNORM", w_format::B8G8R8_UNORM)
			.value("B8G8R8_SNORM", w_format::B8G8R8_SNORM)
			.value("B8G8R8_USCALED", w_format::B8G8R8_USCALED)
			.value("B8G8R8_SSCALED", w_format::B8G8R8_SSCALED)
			.value("B8G8R8_UINT", w_format::B8G8R8_UINT)
			.value("B8G8R8_SINT", w_format::B8G8R8_SINT)
			.value("B8G8R8_SRGB", w_format::B8G8R8_SRGB)
			.value("R8G8B8A8_UNORM", w_format::R8G8B8A8_UNORM)
			.value("R8G8B8A8_SNORM", w_format::R8G8B8A8_SNORM)
			.value("R8G8B8A8_USCALED", w_format::R8G8B8A8_USCALED)
			.value("R8G8B8A8_SSCALED", w_format::R8G8B8A8_SSCALED)
			.value("R8G8B8A8_UINT", w_format::R8G8B8A8_UINT)
			.value("R8G8B8A8_SINT", w_format::R8G8B8A8_SINT)
			.value("R8G8B8A8_SRGB", w_format::R8G8B8A8_SRGB)
			.value("B8G8R8A8_UNORM", w_format::B8G8R8A8_UNORM)
			.value("B8G8R8A8_SNORM", w_format::B8G8R8A8_SNORM)
			.value("B8G8R8A8_USCALED", w_format::B8G8R8A8_USCALED)
			.value("B8G8R8A8_SSCALED", w_format::B8G8R8A8_SSCALED)
			.value("B8G8R8A8_UINT", w_format::B8G8R8A8_UINT)
			.value("B8G8R8A8_SINT", w_format::B8G8R8A8_SINT)
			.value("B8G8R8A8_SRGB", w_format::B8G8R8A8_SRGB)
			.value("A8B8G8R8_UNORM_PACK32", w_format::A8B8G8R8_UNORM_PACK32)
			.value("A8B8G8R8_SNORM_PACK32", w_format::A8B8G8R8_SNORM_PACK32)
			.value("A8B8G8R8_USCALED_PACK32", w_format::A8B8G8R8_USCALED_PACK32)
			.value("A8B8G8R8_SSCALED_PACK32", w_format::A8B8G8R8_SSCALED_PACK32)
			.value("A8B8G8R8_UINT_PACK32", w_format::A8B8G8R8_UINT_PACK32)
			.value("A8B8G8R8_SINT_PACK32", w_format::A8B8G8R8_SINT_PACK32)
			.value("A8B8G8R8_SRGB_PACK32", w_format::A8B8G8R8_SRGB_PACK32)
			.value("A2R10G10B10_UNORM_PACK32", w_format::A2R10G10B10_UNORM_PACK32)
			.value("A2R10G10B10_SNORM_PACK32", w_format::A2R10G10B10_SNORM_PACK32)
			.value("A2R10G10B10_USCALED_PACK32", w_format::A2R10G10B10_USCALED_PACK32)
			.value("A2R10G10B10_SSCALED_PACK32", w_format::A2R10G10B10_SSCALED_PACK32)
			.value("A2R10G10B10_UINT_PACK32", w_format::A2R10G10B10_UINT_PACK32)
			.value("A2R10G10B10_SINT_PACK32", w_format::A2R10G10B10_SINT_PACK32)
			.value("A2B10G10R10_UNORM_PACK32", w_format::A2B10G10R10_UNORM_PACK32)
			.value("A2B10G10R10_SNORM_PACK32", w_format::A2B10G10R10_SNORM_PACK32)
			.value("A2B10G10R10_USCALED_PACK32", w_format::A2B10G10R10_USCALED_PACK32)
			.value("A2B10G10R10_SSCALED_PACK32", w_format::A2B10G10R10_SSCALED_PACK32)
			.value("A2B10G10R10_UINT_PACK32", w_format::A2B10G10R10_UINT_PACK32)
			.value("A2B10G10R10_SINT_PACK32", w_format::A2B10G10R10_SINT_PACK32)
			.value("R16_UNORM", w_format::R16_UNORM)
			.value("R16_SNORM", w_format::R16_SNORM)
			.value("R16_USCALED", w_format::R16_USCALED)
			.value("R16_SSCALED", w_format::R16_SSCALED)
			.value("R16_UINT", w_format::R16_UINT)
			.value("R16_SINT", w_format::R16_SINT)
			.value("R16_SFLOAT", w_format::R16_SFLOAT)
			.value("R16G16_UNORM", w_format::R16G16_UNORM)
			.value("R16G16_SNORM", w_format::R16G16_SNORM)
			.value("R16G16_USCALED", w_format::R16G16_USCALED)
			.value("R16G16_SSCALED", w_format::R16G16_SSCALED)
			.value("R16G16_UINT", w_format::R16G16_UINT)
			.value("R16G16_SINT", w_format::R16G16_SINT)
			.value("R16G16_SFLOAT", w_format::R16G16_SFLOAT)
			.value("R16G16B16_UNORM", w_format::R16G16B16_UNORM)
			.value("R16G16B16_SNORM", w_format::R16G16B16_SNORM)
			.value("R16G16B16_USCALED", w_format::R16G16B16_USCALED)
			.value("R16G16B16_SSCALED", w_format::R16G16B16_SSCALED)
			.value("R16G16B16_UINT", w_format::R16G16B16_UINT)
			.value("R16G16B16_SINT", w_format::R16G16B16_SINT)
			.value("R16G16B16_SFLOAT", w_format::R16G16B16_SFLOAT)
			.value("R16G16B16A16_UNORM", w_format::R16G16B16A16_UNORM)
			.value("R16G16B16A16_SNORM", w_format::R16G16B16A16_SNORM)
			.value("R16G16B16A16_USCALED", w_format::R16G16B16A16_USCALED)
			.value("R16G16B16A16_SSCALED", w_format::R16G16B16A16_SSCALED)
			.value("R16G16B16A16_UINT", w_format::R16G16B16A16_UINT)
			.value("R16G16B16A16_SINT", w_format::R16G16B16A16_SINT)
			.value("R16G16B16A16_SFLOAT", w_format::R16G16B16A16_SFLOAT)
			.value("R32_UINT", w_format::R32_UINT)
			.value("R32_SINT", w_format::R32_SINT)
			.value("R32_SFLOAT", w_format::R32_SFLOAT)
			.value("R32G32_UINT", w_format::R32G32_UINT)
			.value("R32G32_SINT", w_format::R32G32_SINT)
			.value("R32G32_SFLOAT", w_format::R32G32_SFLOAT)
			.value("R32G32B32_UINT", w_format::R32G32B32_UINT)
			.value("R32G32B32_SINT", w_format::R32G32B32_SINT)
			.value("R32G32B32_SFLOAT", w_format::R32G32B32_SFLOAT)
			.value("R32G32B32A32_UINT", w_format::R32G32B32A32_UINT)
			.value("R32G32B32A32_SINT", w_format::R32G32B32A32_SINT)
			.value("R32G32B32A32_SFLOAT", w_format::R32G32B32A32_SFLOAT)
			.value("R64_UINT", w_format::R64_UINT)
			.value("R64_SINT", w_format::R64_SINT)
			.value("R64_SFLOAT", w_format::R64_SFLOAT)
			.value("R64G64_UINT", w_format::R64G64_UINT)
			.value("R64G64_SINT", w_format::R64G64_SINT)
			.value("R64G64_SFLOAT", w_format::R64G64_SFLOAT)
			.value("R64G64B64_UINT", w_format::R64G64B64_UINT)
			.value("R64G64B64_SINT", w_format::R64G64B64_SINT)
			.value("R64G64B64_SFLOAT", w_format::R64G64B64_SFLOAT)
			.value("R64G64B64A64_UINT", w_format::R64G64B64A64_UINT)
			.value("R64G64B64A64_SINT", w_format::R64G64B64A64_SINT)
			.value("R64G64B64A64_SFLOAT", w_format::R64G64B64A64_SFLOAT)
			.value("B10G11R11_UFLOAT_PACK32", w_format::B10G11R11_UFLOAT_PACK32)
			.value("E5B9G9R9_UFLOAT_PACK32", w_format::E5B9G9R9_UFLOAT_PACK32)
			.value("D16_UNORM", w_format::D16_UNORM)
			.value("X8_D24_UNORM_PACK32", w_format::X8_D24_UNORM_PACK32)
			.value("D32_SFLOAT", w_format::D32_SFLOAT)
			.value("S8_UINT", w_format::S8_UINT)
			.value("D16_UNORM_S8_UINT", w_format::D16_UNORM_S8_UINT)
			.value("D24_UNORM_S8_UINT", w_format::D24_UNORM_S8_UINT)
			.value("D32_SFLOAT_S8_UINT", w_format::D32_SFLOAT_S8_UINT)
			.value("BC1_RGB_UNORM_BLOCK", w_format::BC1_RGB_UNORM_BLOCK)
			.value("BC1_RGB_SRGB_BLOCK", w_format::BC1_RGB_SRGB_BLOCK)
			.value("BC1_RGBA_UNORM_BLOCK", w_format::BC1_RGBA_UNORM_BLOCK)
			.value("BC1_RGBA_SRGB_BLOCK", w_format::BC1_RGBA_SRGB_BLOCK)
			.value("BC2_UNORM_BLOCK", w_format::BC2_UNORM_BLOCK)
			.value("BC2_SRGB_BLOCK", w_format::BC2_SRGB_BLOCK)
			.value("BC3_UNORM_BLOCK", w_format::BC3_UNORM_BLOCK)
			.value("BC3_SRGB_BLOCK", w_format::BC3_SRGB_BLOCK)
			.value("BC4_UNORM_BLOCK", w_format::BC4_UNORM_BLOCK)
			.value("BC5_UNORM_BLOCK", w_format::BC5_UNORM_BLOCK)
			.value("BC5_SNORM_BLOCK", w_format::BC5_SNORM_BLOCK)
			.value("BC6H_UFLOAT_BLOCK", w_format::BC6H_UFLOAT_BLOCK)
			.value("BC6H_SFLOAT_BLOCK", w_format::BC6H_SFLOAT_BLOCK)
			.value("BC7_UNORM_BLOCK", w_format::BC7_UNORM_BLOCK)
			.value("BC7_SRGB_BLOCK", w_format::BC7_SRGB_BLOCK)
			.value("ETC2_R8G8B8_UNORM_BLOCK", w_format::ETC2_R8G8B8_UNORM_BLOCK)
			.value("ETC2_R8G8B8_SRGB_BLOCK", w_format::ETC2_R8G8B8_SRGB_BLOCK)
			.value("ETC2_R8G8B8A1_UNORM_BLOCK", w_format::ETC2_R8G8B8A1_UNORM_BLOCK)
			.value("ETC2_R8G8B8A1_SRGB_BLOCK", w_format::ETC2_R8G8B8A1_SRGB_BLOCK)
			.value("ETC2_R8G8B8A8_UNORM_BLOCK", w_format::ETC2_R8G8B8A8_UNORM_BLOCK)
			.value("ETC2_R8G8B8A8_SRGB_BLOCK", w_format::ETC2_R8G8B8A8_SRGB_BLOCK)
			.value("EAC_R11_UNORM_BLOCK", w_format::EAC_R11_UNORM_BLOCK)
			.value("EAC_R11_SNORM_BLOCK", w_format::EAC_R11_SNORM_BLOCK)
			.value("EAC_R11G11_UNORM_BLOCK", w_format::EAC_R11G11_UNORM_BLOCK)
			.value("EAC_R11G11_SNORM_BLOCK", w_format::EAC_R11G11_SNORM_BLOCK)
			.value("ASTC_4x4_UNORM_BLOCK", w_format::ASTC_4x4_UNORM_BLOCK)
			.value("ASTC_4x4_SRGB_BLOCK", w_format::ASTC_4x4_SRGB_BLOCK)
			.value("ASTC_5x4_UNORM_BLOCK", w_format::ASTC_5x4_UNORM_BLOCK)
			.value("ASTC_5x4_SRGB_BLOCK", w_format::ASTC_5x4_SRGB_BLOCK)
			.value("ASTC_5x5_UNORM_BLOCK", w_format::ASTC_5x5_UNORM_BLOCK)
			.value("ASTC_5x5_SRGB_BLOCK", w_format::ASTC_5x5_SRGB_BLOCK)
			.value("ASTC_6x5_UNORM_BLOCK", w_format::ASTC_6x5_UNORM_BLOCK)
			.value("ASTC_6x5_SRGB_BLOCK", w_format::ASTC_6x5_SRGB_BLOCK)
			.value("ASTC_6x6_UNORM_BLOCK", w_format::ASTC_6x6_UNORM_BLOCK)
			.value("ASTC_6x6_SRGB_BLOCK", w_format::ASTC_6x6_SRGB_BLOCK)
			.value("ASTC_8x5_UNORM_BLOCK", w_format::ASTC_8x5_UNORM_BLOCK)
			.value("ASTC_8x5_SRGB_BLOCK", w_format::ASTC_8x5_SRGB_BLOCK)
			.value("ASTC_8x6_UNORM_BLOCK", w_format::ASTC_8x6_UNORM_BLOCK)
			.value("ASTC_8x6_SRGB_BLOCK", w_format::ASTC_8x6_SRGB_BLOCK)
			.value("ASTC_8x8_UNORM_BLOCK", w_format::ASTC_8x8_UNORM_BLOCK)
			.value("ASTC_8x8_SRGB_BLOCK", w_format::ASTC_8x8_SRGB_BLOCK)
			.value("ASTC_10x5_UNORM_BLOCK", w_format::ASTC_10x5_UNORM_BLOCK)
			.value("ASTC_10x5_SRGB_BLOCK", w_format::ASTC_10x5_SRGB_BLOCK)
			.value("ASTC_10x6_UNORM_BLOCK", w_format::ASTC_10x6_UNORM_BLOCK)
			.value("ASTC_10x6_SRGB_BLOCK", w_format::ASTC_10x6_SRGB_BLOCK)
			.value("ASTC_10x8_UNORM_BLOCK", w_format::ASTC_10x8_UNORM_BLOCK)
			.value("ASTC_10x8_SRGB_BLOCK", w_format::ASTC_10x8_SRGB_BLOCK)
			.value("ASTC_10x10_UNORM_BLOCK", w_format::ASTC_10x10_UNORM_BLOCK)
			.value("ASTC_10x10_SRGB_BLOCK", w_format::ASTC_10x10_SRGB_BLOCK)
			.value("ASTC_12x10_UNORM_BLOCK", w_format::ASTC_12x10_UNORM_BLOCK)
			.value("ASTC_12x10_SRGB_BLOCK", w_format::ASTC_12x10_SRGB_BLOCK)
			.value("ASTC_12x12_UNORM_BLOCK", w_format::ASTC_12x12_UNORM_BLOCK)
			.value("ASTC_12x12_SRGB_BLOCK", w_format::ASTC_12x12_SRGB_BLOCK)
			.value("PVRTC1_2BPP_UNORM_BLOCK_IMG", w_format::PVRTC1_2BPP_UNORM_BLOCK_IMG)
			.value("PVRTC1_4BPP_UNORM_BLOCK_IMG", w_format::PVRTC1_4BPP_UNORM_BLOCK_IMG)
			.value("PVRTC2_2BPP_UNORM_BLOCK_IMG", w_format::PVRTC2_2BPP_UNORM_BLOCK_IMG)
			.value("PVRTC2_4BPP_UNORM_BLOCK_IMG", w_format::PVRTC2_4BPP_UNORM_BLOCK_IMG)
			.value("PVRTC1_2BPP_SRGB_BLOCK_IMG", w_format::PVRTC1_2BPP_SRGB_BLOCK_IMG)
			.value("PVRTC1_4BPP_SRGB_BLOCK_IMG", w_format::PVRTC1_4BPP_SRGB_BLOCK_IMG)
			.value("PVRTC2_2BPP_SRGB_BLOCK_IMG", w_format::PVRTC2_2BPP_SRGB_BLOCK_IMG)
			.value("PVRTC2_4BPP_SRGB_BLOCK_IMG", w_format::PVRTC2_4BPP_SRGB_BLOCK_IMG)
			.value("G8B8G8R8_422_UNORM_KHR", w_format::G8B8G8R8_422_UNORM_KHR)
			.value("B8G8R8G8_422_UNORM_KHR", w_format::B8G8R8G8_422_UNORM_KHR)
			.value("G8_B8_R8_3PLANE_420_UNORM_KHR", w_format::G8_B8_R8_3PLANE_420_UNORM_KHR)
			.value("G8_B8R8_2PLANE_420_UNORM_KHR", w_format::G8_B8R8_2PLANE_420_UNORM_KHR)
			.value("G8_B8_R8_3PLANE_422_UNORM_KHR", w_format::G8_B8_R8_3PLANE_422_UNORM_KHR)
			.value("G8_B8R8_2PLANE_422_UNORM_KHR", w_format::G8_B8R8_2PLANE_422_UNORM_KHR)
			.value("G8_B8_R8_3PLANE_444_UNORM_KHR", w_format::G8_B8_R8_3PLANE_444_UNORM_KHR)
			.value("R10X6_UNORM_PACK16_KHR", w_format::R10X6_UNORM_PACK16_KHR)
			.value("R10X6G10X6_UNORM_2PACK16_KHR", w_format::R10X6G10X6_UNORM_2PACK16_KHR)
			.value("R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR", w_format::R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR)
			.value("G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR", w_format::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR)
			.value("B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR", w_format::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR)
			.value("G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR", w_format::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR)
			.value("G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR", w_format::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR)
			.value("G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR", w_format::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR)
			.value("G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR", w_format::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR)
			.value("G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR", w_format::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR)
			.value("R12X4_UNORM_PACK16_KHR", w_format::R12X4_UNORM_PACK16_KHR)
			.value("R12X4G12X4_UNORM_2PACK16_KHR", w_format::R12X4G12X4_UNORM_2PACK16_KHR)
			.value("R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR", w_format::R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR)
			.value("G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR", w_format::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR)
			.value("B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR", w_format::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR)
			.value("G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR", w_format::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR)
			.value("G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR", w_format::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR)
			.value("G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR", w_format::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR)
			.value("G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR", w_format::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR)
			.value("G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR", w_format::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR)
			.value("G16B16G16R16_422_UNORM_KHR", w_format::G16B16G16R16_422_UNORM_KHR)
			.value("B16G16R16G16_422_UNORM_KHR", w_format::B16G16R16G16_422_UNORM_KHR)
			.value("G16_B16_R16_3PLANE_420_UNORM_KHR", w_format::G16_B16_R16_3PLANE_420_UNORM_KHR)
			.value("G16_B16R16_2PLANE_420_UNORM_KHR", w_format::G16_B16R16_2PLANE_420_UNORM_KHR)
			.value("G16_B16_R16_3PLANE_422_UNORM_KHR", w_format::G16_B16_R16_3PLANE_422_UNORM_KHR)
			.value("G16_B16R16_2PLANE_422_UNORM_KHR", w_format::G16_B16R16_2PLANE_422_UNORM_KHR)
			.value("G16_B16_R16_3PLANE_444_UNORM_KHR", w_format::G16_B16_R16_3PLANE_444_UNORM_KHR)
			.export_values()
			;


			//export w_image_view class
			class_<w_image_view>("w_image_view", init<>())
				.add_property("width", &w_image_view::width, "width of image view")
				.add_property("height", &w_image_view::height, "height of image view")
				.def_readwrite("attachment_desc", &w_image_view::attachment_desc, "attachment description")
				;

			//export w_output_presentation_window class
			class_<w_output_presentation_window, boost::noncopyable>("w_output_presentation_window", init<>())
				.add_property("swap_chain_image_index", &w_output_presentation_window::swap_chain_image_index, "get swap chain image index")
				.add_property("swap_chain_image_views", &w_output_presentation_window::py_get_swap_chain_image_views, "get swap chain image view")
				.add_property("depth_buffer_image_view", &w_output_presentation_window::depth_buffer_image_view, "get depth buffer image view")
				.add_property("depth_buffer_format", &w_output_presentation_window::depth_buffer_format, "get depth buffer format")
				.add_property("swap_chain_image_is_available_semaphore", &w_output_presentation_window::swap_chain_image_is_available_semaphore, "semaphore for checking whether swap chain's image is available or not")
				.add_property("rendering_done_semaphore", &w_output_presentation_window::rendering_done_semaphore, "semaphore for signaling, when all rendering is done")
				.add_property("width", &w_output_presentation_window::width, "get width of presentation window")
				.add_property("height", &w_output_presentation_window::height, "get height of presentation window")
				.add_property("v_sync", &w_output_presentation_window::v_sync, "get status of v-sync")
				.add_property("cpu_access_to_swapchain_buffer", &w_output_presentation_window::cpu_access_to_swapchain_buffer, "get whether cpu allowed to access swap chain")
				.add_property("double_buffering", &w_output_presentation_window::double_buffering, "get statis of double buffering")
				;

			//export w_graphics_device class
			class_<w_graphics_device, boost::noncopyable>("w_graphics_device")
				.def("get_info", &w_graphics_device::get_info, "print graphics device information")
				.def("get_number_of_swap_chains", &w_graphics_device::get_number_of_swap_chains, "get number of swap chain buffers")
				.def("draw", &w_graphics_device::py_draw, "draw primitive(s) and instances using vertex & index buffer")
				.def("submit", &w_graphics_device::py_submit, "submit command buffer")
				.add_property("output_presentation_window", &w_graphics_device::py_get_output_presentation_window, "get shared pointer to the output presentation window structure")
				.add_property("graphics_queue", &w_graphics_device::vk_graphics_queue, "get graphics queue")
				.add_property("present_queue", &w_graphics_device::vk_present_queue, "get present queue")
				.add_property("compute_queue", &w_graphics_device::vk_compute_queue, "get compute queue")
				.add_property("transfer_queue", &w_graphics_device::vk_transfer_queue, "get transfer queue")
				.add_property("sparse_queue", &w_graphics_device::vk_sparse_queue, "get sparse queue")
				;

			register_ptr_to_python< boost::shared_ptr<w_output_presentation_window>>();
	}
}

#endif//__PY_GRAPHICS_DEVICE_MANAGER_H__

#endif//__PYTHON__
