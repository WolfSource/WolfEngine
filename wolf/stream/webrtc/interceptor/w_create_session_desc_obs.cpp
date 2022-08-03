#include "w_create_session_desc_obs.hpp"
#include <wolf.hpp>

using w_create_session_desc_obs = wolf::stream::webRTC::w_create_session_desc_obs;

w_create_session_desc_obs::w_create_session_desc_obs(
	const rtc::scoped_refptr<webrtc::PeerConnectionInterface>& p_pc,
	std::promise<const webrtc::SessionDescriptionInterface*>& p_promise) :
	_pc(p_pc), _promise(p_promise)
{
}

w_create_session_desc_obs* w_create_session_desc_obs::create(
	const rtc::scoped_refptr<webrtc::PeerConnectionInterface>& p_pc,
	std::promise<const webrtc::SessionDescriptionInterface*>& p_promise)
{
	return new rtc::RefCountedObject<w_create_session_desc_obs>(p_pc, p_promise);
}

void w_create_session_desc_obs::OnSuccess(webrtc::SessionDescriptionInterface* p_desc)
{
	std::string sdp;
	p_desc->ToString(&sdp);

	if (!this->_cancelled)
	{
		// auto* _session = wolf::stream::webRTC::w_create_session_desc_obs::create(
		// 	this->_pc,
		// 	this->_promise);
	}
}

void w_create_session_desc_obs::OnFailure(webrtc::RTCError p_error)
{
	W_UNUSED(p_error);
	if (!this->_cancelled)
	{
		this->_promise.set_value(nullptr);
	}
}

void w_create_session_desc_obs::cancel()
{
	this->_cancelled = true;
}
