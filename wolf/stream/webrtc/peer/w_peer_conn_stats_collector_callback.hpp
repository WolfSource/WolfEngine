/*
	Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/scoped_refptr.h>
#include <api/stats/rtc_stats_report.h>
#include <api/stats/rtc_stats_collector_callback.h>
#include <rapidjson/document.h> 

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
	class w_peer_conn_stats_collector_callback : public webrtc::RTCStatsCollectorCallback
	{
	public:
		w_peer_conn_stats_collector_callback() = default;
		void clear_report();
		rapidjson::Document& get_report();

	protected:
		void OnStatsDelivered(const rtc::scoped_refptr<const webrtc::RTCStatsReport>& p_report) override;

		rapidjson::Document _report;
	};
} // namespace wolf::stream::webRTC