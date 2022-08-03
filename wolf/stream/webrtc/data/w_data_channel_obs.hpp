/*
	Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/data_channel_interface.h>
#include <api/scoped_refptr.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
	class w_data_channel_obs : public webrtc::DataChannelObserver
	{
	public:
		w_data_channel_obs(rtc::scoped_refptr<webrtc::DataChannelInterface>
							   p_data_channel);
		virtual ~w_data_channel_obs();

		void OnStateChange() override;
		void OnMessage(const webrtc::DataBuffer &p_buffer) override;

	protected:
		rtc::scoped_refptr<webrtc::DataChannelInterface> _data_channel;
	};
} // namespace wolf::stream::webRTC