#include "w_set_session_desc_obs.hpp"
#include <wolf.hpp>

using w_set_session_desc_obs = wolf::stream::webRTC::w_set_session_desc_obs;

w_set_session_desc_obs::w_set_session_desc_obs(
    const rtc::scoped_refptr<webrtc::PeerConnectionInterface>& p_pc,
    std::promise<const webrtc::SessionDescriptionInterface*>& p_promise)
    : _pc(p_pc), _promise(p_promise)
{

}

w_set_session_desc_obs* w_set_session_desc_obs::create(
    const rtc::scoped_refptr<webrtc::PeerConnectionInterface>& p_pc,
    std::promise<const webrtc::SessionDescriptionInterface*>& p_promise)
{
    return new rtc::RefCountedObject<w_set_session_desc_obs>(
        p_pc, p_promise);
}

void w_set_session_desc_obs::OnSuccess()
{
    std::string sdp;
    if (!this->_cancelled)
    {
        if (this->_pc->local_description())
        {
            this->_promise.set_value(this->_pc->local_description());
            this->_pc->local_description()->ToString(&sdp);
        }
        else if (this->_pc->remote_description())
        {
            this->_promise.set_value(this->_pc->remote_description());
            this->_pc->remote_description()->ToString(&sdp);
        }
    }
}

void w_set_session_desc_obs::OnFailure(webrtc::RTCError p_error)
{
    W_UNUSED(p_error);

    if (!this->_cancelled)
    {
        this->_promise.set_value(nullptr);
    }
}

void w_set_session_desc_obs::cancel()
{
    this->_cancelled = true;
}