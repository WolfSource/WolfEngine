#include "w_peer_conn_stats_collector_callback.hpp"

using w_peer_conn_stats_collector_callback = wolf::stream::webRTC::w_peer_conn_stats_collector_callback;

void w_peer_conn_stats_collector_callback::clear_report()
{
	this->_report.Clear();
}

rapidjson::Document& w_peer_conn_stats_collector_callback::get_report()
{ 
	return this->_report;
}

void w_peer_conn_stats_collector_callback::OnStatsDelivered(
	const rtc::scoped_refptr<const webrtc::RTCStatsReport>& p_report)
{
	for (const webrtc::RTCStats& stats : *p_report)
	{
		rapidjson::Value stats_members = {};
		for (const auto* _member : stats.Members())
		{
			auto _key = _member->name();
			auto _value = _member->ValueToString();

			stats_members[_key].SetString(_value.c_str(), _value.length());
		}
		auto _name = stats.id().c_str();
		this->_report[_name] = stats_members;
	}
}