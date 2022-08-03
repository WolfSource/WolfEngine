/*
	Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>
#include <future>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/peer_connection_interface.h>
#include <api/scoped_refptr.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
	class w_create_session_desc_obs : public webrtc::CreateSessionDescriptionObserver
	{
	public:
		static w_create_session_desc_obs* create(
			const rtc::scoped_refptr<webrtc::PeerConnectionInterface>& p_pc,
			std::promise<const webrtc::SessionDescriptionInterface*>& p_promise);

		void OnSuccess(webrtc::SessionDescriptionInterface* p_desc) override;
		void OnFailure(webrtc::RTCError p_error) override;

		void cancel();

	protected:
		w_create_session_desc_obs(const rtc::scoped_refptr<webrtc::PeerConnectionInterface>& p_pc,
			std::promise<const webrtc::SessionDescriptionInterface*>& p_promise);

	private:
		rtc::scoped_refptr<webrtc::PeerConnectionInterface> _pc;
		std::promise<const webrtc::SessionDescriptionInterface*>& _promise;
		bool _cancelled = false;
	};
} // namespace wolf::stream::webRTC