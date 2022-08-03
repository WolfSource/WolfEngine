#include "w_data_channel_obs.hpp"
#include <wolf.hpp>

using w_data_channel_obs = wolf::stream::webRTC::w_data_channel_obs;

w_data_channel_obs::w_data_channel_obs(rtc::scoped_refptr<webrtc::DataChannelInterface>
	p_data_channel) : _data_channel(p_data_channel)
{
	this->_data_channel->RegisterObserver(this);
}

w_data_channel_obs::~w_data_channel_obs()
{
	this->_data_channel->UnregisterObserver();
}

void w_data_channel_obs::OnStateChange()
{
	std::string msg(this->_data_channel->label() + " " +
		webrtc::DataChannelInterface::DataStateString(this->_data_channel->state()));

	webrtc::DataBuffer buffer(msg);
	this->_data_channel->Send(buffer);
}

void w_data_channel_obs::OnMessage(const webrtc::DataBuffer& p_buffer)
{
	W_UNUSED(p_buffer);
	//std::string msg((const char*)p_buffer.data.data(), p_buffer.data.size());
}