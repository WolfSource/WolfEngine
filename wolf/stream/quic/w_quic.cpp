#ifdef WOLF_STREAM_QUIC

#include "w_quic.hpp"
#include <memory>
#include <variant>

#if __linux__
#   include <net/if.h>  // For IFNAMSIZ
#endif

#if __linux__ || defined(__FreeBSD__)
#define ECN_SUPPORTED 1
#else
#define ECN_SUPPORTED 0
#endif

#ifndef LSQUIC_DONTFRAG_SUPPORTED
#define LSQUIC_DONTFRAG_SUPPORTED (HAVE_IP_DONTFRAG || HAVE_IP_MTU_DISCOVER || HAVE_IPV6_MTU_DISCOVER)
#endif

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <MSWSock.h>
#include "vc_compat.h"
#include <system/getopt.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <lsquic.h>
#include <liblsquic/lsquic_byteswap.h>
#include <sys/queue.h>

#ifdef WIN32
#define SOCKOPT_VAL DWORD
#define SOCKET_TYPE SOCKET
#define CLOSE_SOCKET closesocket
#define CHAR_CAST (char *)

LPFN_WSARECVMSG pfnWSARecvMsg;
GUID recvGuid = WSAID_WSARECVMSG;
LPFN_WSASENDMSG pfnWSASendMsg;
GUID sendGuid = WSAID_WSASENDMSG;

#else
#define SOCKOPT_VAL int
#define SOCKET_TYPE int
#define CLOSE_SOCKET close
#define CHAR_CAST
#endif

using w_quic = wolf::stream::quic::w_quic;

static const struct lsquic_packout_mem_if s_pmi = 
{
	.pmi_allocate = pba_allocate,
	.pmi_release = pba_release,
	.pmi_return = pba_release,
};

enum ctl_what
{
	CTL_W_UNKOWN = 0,
	CTL_W_SENDADDR = 1 << 0,
#if ECN_SUPPORTED
	CTL_W_ECN = 1 << 1,
#endif
};

enum service_port_flags
{
#if LSQUIC_DONTFRAG_SUPPORTED
    W_SERVICE_PORT_FRAGMENT_OK = (1 << 0),
#endif
    W_SERVICE_PORT_SET_SNDBUF = (1 << 1), /* SO_SNDBUF */
    W_SERVICE_PORT_SET_RCVBUF = (1 << 2), /* SO_RCVBUF */
    W_SERVICE_PORT_SERVER = (1 << 3),
    W_SERVICE_PORT_CONNECT = (1 << 4),
};

struct packets_in
{
	unsigned char* packet_data;
	unsigned char* ctlmsg_data;
#ifdef WIN32
	WSABUF* vecs;
#else
	struct iovec* vecs;
#endif

#if ECN_SUPPORTED
	int* ecn;
#endif
	sockaddr_storage* local_addresses;
	sockaddr_storage* peer_addresses;
	unsigned                 n_alloc;
	unsigned                 data_sz;
};

struct service_port
{
	TAILQ_ENTRY(service_port)   next_sport;
#ifndef WIN32
	int                         fd;
#else
	SOCKET                      fd;
#endif
#if __linux__
	uint32_t                    n_dropped;
	int                         drop_init;
	char                        if_name[IFNAMSIZ];
#endif
	struct event* ev;
	struct lsquic_engine* engine;
	void* conn_ctx;
	char                        host[80];
	sockaddr_storage     sas;
	sockaddr_storage     sp_local_addr;
	packets_in* packs_in;
	service_port_flags          sp_flags;
	SOCKOPT_VAL                 sp_sndbuf;   /* If SPORT_SET_SNDBUF is set */
	SOCKOPT_VAL                 sp_rcvbuf;   /* If SPORT_SET_RCVBUF is set */
	unsigned char* sp_token_buf;
	size_t                      sp_token_sz;
};

TAILQ_HEAD(sport_head, service_port);

struct w_quic::ctx
{
	ctx(w_quic_flags p_flags) :
		flags(p_flags)
	{
	}

	~ctx()
	{
	}

	w_quic_flags flags;
	lsquic_engine_t* engine;
	sport_head sports;
	lsquic_engine_settings settings;
};

struct lsquic_conn_ctx 
{
	w_quic* quic;
};

struct lsquic_stream_ctx
{
	// Number of bytes left to write or Read client header in
	std::variant < uint64_t, std::array<uint8_t, sizeof(uint64_t)>> u;
	// Number of header bytes read in
	uint32_t n_h_read;
};

static void s_setup_control_msg(
#ifdef WIN32
	WSAMSG
#else
	struct msghdr
#endif
	* msg, ctl_what cw,
	const lsquic_out_spec* spec, 
	unsigned char* buf, 
	size_t bufsz)
{
	struct cmsghdr* cmsg;
	struct sockaddr_in* local_sa;
	struct sockaddr_in6* local_sa6;
#if __linux__ || __APPLE__ || WIN32
	struct in_pktinfo info;
#endif
	struct in6_pktinfo info6;
	size_t ctl_len;

#ifndef WIN32
	msg->msg_control = buf;
	msg->msg_controllen = bufsz;
#else
	msg->Control.buf = (char*)buf;
	msg->Control.len = bufsz;
#endif

	/* Need to zero the buffer due to a bug(?) in CMSG_NXTHDR.  See
	 * https://stackoverflow.com/questions/27601849/cmsg-nxthdr-returns-null-even-though-there-are-more-cmsghdr-objects
	 */
	memset(buf, 0, bufsz);

	ctl_len = 0;
	for (cmsg = CMSG_FIRSTHDR(msg); cw && cmsg; cmsg = CMSG_NXTHDR(msg, cmsg))
	{
		if (cw & ctl_what::CTL_W_SENDADDR)
		{
			if (AF_INET == spec->dest_sa->sa_family)
			{
				local_sa = (struct sockaddr_in*)spec->local_sa;
#if __linux__ || __APPLE__
				memset(&info, 0, sizeof(info));
				info.ipi_spec_dst = local_sa->sin_addr;
				cmsg->cmsg_level = IPPROTO_IP;
				cmsg->cmsg_type = IP_PKTINFO;
				cmsg->cmsg_len = CMSG_LEN(sizeof(info));
				ctl_len += CMSG_SPACE(sizeof(info));
				memcpy(CMSG_DATA(cmsg), &info, sizeof(info));
#elif WIN32
				memset(&info, 0, sizeof(info));
				info.ipi_addr = local_sa->sin_addr;
				cmsg->cmsg_level = IPPROTO_IP;
				cmsg->cmsg_type = IP_PKTINFO;
				cmsg->cmsg_len = CMSG_LEN(sizeof(info));
				ctl_len += CMSG_SPACE(sizeof(info));
				memcpy(WSA_CMSG_DATA(cmsg), &info, sizeof(info));
#else
				cmsg->cmsg_level = IPPROTO_IP;
				cmsg->cmsg_type = IP_SENDSRCADDR;
				cmsg->cmsg_len = CMSG_LEN(sizeof(local_sa->sin_addr));
				ctl_len += CMSG_SPACE(sizeof(local_sa->sin_addr));
				memcpy(CMSG_DATA(cmsg), &local_sa->sin_addr,
					sizeof(local_sa->sin_addr));
#endif
			}
			else
			{
				local_sa6 = (struct sockaddr_in6*)spec->local_sa;
				memset(&info6, 0, sizeof(info6));
				info6.ipi6_addr = local_sa6->sin6_addr;
				cmsg->cmsg_level = IPPROTO_IPV6;
				cmsg->cmsg_type = IPV6_PKTINFO;
				cmsg->cmsg_len = CMSG_LEN(sizeof(info6));
#ifndef WIN32
				memcpy(CMSG_DATA(cmsg), &info6, sizeof(info6));
#else
				memcpy(WSA_CMSG_DATA(cmsg), &info6, sizeof(info6));
#endif
				ctl_len += CMSG_SPACE(sizeof(info6));
			}

			// do bitwise
			int cw_int = gsl::narrow_cast<int>(cw);
			cw_int &= ~ctl_what::CTL_W_SENDADDR;

			cw = gsl::narrow_cast<ctl_what>(cw_int);
		}
#if ECN_SUPPORTED
		else if (cw & CW_ECN)
		{
			if (AF_INET == spec->dest_sa->sa_family)
			{
				const
#if defined(__FreeBSD__)
					unsigned char
#else
					int
#endif
					tos = spec->ecn;
				cmsg->cmsg_level = IPPROTO_IP;
				cmsg->cmsg_type = IP_TOS;
				cmsg->cmsg_len = CMSG_LEN(sizeof(tos));
				memcpy(CMSG_DATA(cmsg), &tos, sizeof(tos));
				ctl_len += CMSG_SPACE(sizeof(tos));
			}
			else
			{
				const int tos = spec->ecn;
				cmsg->cmsg_level = IPPROTO_IPV6;
				cmsg->cmsg_type = IPV6_TCLASS;
				cmsg->cmsg_len = CMSG_LEN(sizeof(tos));
				memcpy(CMSG_DATA(cmsg), &tos, sizeof(tos));
				ctl_len += CMSG_SPACE(sizeof(tos));
			}
			cw &= ~CW_ECN;
		}
#endif
		else
			assert(0);
	}

#ifndef WIN32
	msg->msg_controllen = ctl_len;
#else
	msg->Control.len = ctl_len;
#endif
}

static unsigned s_packet_out_limit()
{
	auto* _env = getenv("LSQUIC_PACKET_OUT_LIMIT");
	if (_env)
	{
		return atoi(_env);
	}
	else
	{
		return 0;
	}
}

static int s_send_packets_one_by_one(const lsquic_out_spec* p_specs, unsigned p_count)
{
	ctl_what _ctl_w;
	uint32_t _n;
	int _s = 0;

#ifdef WIN32
	DWORD _bytes;
	WSAMSG _msg;
#else
	struct msghdr _msg;
#endif

	union {
		/* cmsg(3) recommends union for proper alignment */
#if __linux__ || WIN32
#	define SIZE1 sizeof(struct in_pktinfo)
#else
#	define SIZE1 sizeof(struct in_addr)
#endif
		unsigned char buf[
			CMSG_SPACE(std::max(SIZE1, sizeof(struct in6_pktinfo)))
#if ECN_SUPPORTED
				+ CMSG_SPACE(sizeof(int))
#endif
		];
		struct cmsghdr cmsg;
	} ancil;


	uintptr_t _ancil_key, _prev_ancil_key;

	if (0 == p_count)
	{
		return 0;
	}

	const uint32_t _orig_count = p_count;
	const uint32_t _out_limit = s_packet_out_limit();
	if (_out_limit && p_count > _out_limit)
	{
		p_count = _out_limit;
	}

	_n = 0;
	_prev_ancil_key = 0;

#ifdef WIN32

#define MAX_OUT_BATCH_SIZE 1024
	auto _p_wsa_buf = (LPWSABUF)malloc(sizeof(LPWSABUF) * MAX_OUT_BATCH_SIZE * 2);
	if (nullptr == _p_wsa_buf)
	{
		return -1;
	}
#endif

	do
	{
		auto _serv_port = gsl::narrow_cast<const service_port*>(p_specs[_n].peer_ctx);

#ifdef WIN32
		for (int i = 0; i < p_specs[_n].iovlen; i++)
		{
			_p_wsa_buf[i].buf = gsl::narrow_cast<CHAR*>(p_specs[_n].iov[i].iov_base);
			_p_wsa_buf[i].len = p_specs[_n].iov[i].iov_len;
		}

		auto void_ptr = (void*)p_specs[_n].dest_sa;
		_msg.name = gsl::narrow_cast<LPSOCKADDR>(void_ptr);
		_msg.namelen = (AF_INET == p_specs[_n].dest_sa->sa_family ?
			sizeof(sockaddr_in) :
			sizeof(sockaddr_in6));
		_msg.dwBufferCount = p_specs[_n].iovlen;
		_msg.lpBuffers = _p_wsa_buf;
		_msg.dwFlags = 0;
#else
		msg.msg_name = (void*)specs[n].dest_sa;
		msg.msg_namelen = (AF_INET == specs[n].dest_sa->sa_family ?
			sizeof(struct sockaddr_in) :
			sizeof(struct sockaddr_in6)),
			msg.msg_iov = specs[n].iov;
		msg.msg_iovlen = specs[n].iovlen;
		msg.msg_flags = 0;
#endif
		if ((_serv_port->sp_flags & service_port_flags::W_SERVICE_PORT_SERVER) &&
			p_specs[_n].local_sa->sa_family)
		{
			_ctl_w = ctl_what::CTL_W_SENDADDR;
			_ancil_key = (uintptr_t)p_specs[_n].local_sa;
			assert(0 == (_ancil_key & 3));
		}
		else
		{
			_ctl_w = ctl_what::CTL_W_UNKOWN;
			_ancil_key = 0;
		}
#if ECN_SUPPORTED
		if (sport->sp_prog->prog_api.ea_settings->es_ecn && specs[n].ecn)
		{
			cw |= CW_ECN;
			ancil_key |= specs[n].ecn;
		}
#endif
		if (_ctl_w && _prev_ancil_key == _ancil_key)
		{
			/* Reuse previous ancillary message */
		}
		else if (_ctl_w)
		{
			_prev_ancil_key = _ancil_key;
			s_setup_control_msg(
				&_msg,
				_ctl_w,
				&p_specs[_n],
				ancil.buf,
				sizeof(ancil.buf));
		}
		else
		{
			_prev_ancil_key = 0;
#ifdef WIN32
			_msg.Control.buf = nullptr;
			_msg.Control.len = 0;
#else
			_msg.msg_control = nullptr;
			_msg.msg_controllen = 0;
#endif
		}

#ifdef WIN32
		_s = pfnWSASendMsg(_serv_port->fd, &_msg, 0, &_bytes, nullptr, nullptr);
#else
		_s = sendmsg(sport->fd, &msg, 0);
#endif
		if (_s < 0)
		{
#ifdef WIN32
			//LSQ_INFO("sendto failed: %s", WSAGetLastError());
#else
			//LSQ_INFO("sendto failed: %s", strerror(errno));
#endif
			break;
		}
		++_n;
	} while (_n < p_count);

	if (_n < _orig_count)
	{
		//LSQ_DEBUG("cannot send: register on_write event");
	}

#ifdef WIN32
	if (nullptr != _p_wsa_buf)
	{
		free(_p_wsa_buf);
		_p_wsa_buf = nullptr;
	}
#endif

	if (_n > 0)
	{
		if (_n < _orig_count && _out_limit)
		{
			errno = EAGAIN;
		}
		return _n;
	}
	else
	{
		assert(_s < 0);
		return -1;
	}
}

static int s_sport_packets_out(void* p_ctx, const lsquic_out_spec* p_specs, unsigned p_count)
{
#if HAVE_SENDMMSG
	return s_send_packets_using_sendmmsg(p_specs, p_count);
#endif
	return s_send_packets_one_by_one(p_specs, p_count);
}

static lsquic_conn_ctx_t* s_on_conn_new(void* p_stream_if_ctx, lsquic_conn_t* p_conn)
{
	// new connection
	auto _quic = gsl::narrow_cast<w_quic*>(p_stream_if_ctx);
	if (_quic != nullptr)
	{
		_quic->on_connection_new_sig();
	}

	auto _ctx = gsl::narrow_cast<lsquic_conn_ctx*>(malloc(sizeof(lsquic_conn_ctx)));
	if (_ctx != nullptr)
	{
		_ctx->quic = _quic;
	}
	return _ctx;
}

static void s_on_conn_closed(lsquic_conn_t* p_conn)
{
	// connection just closed
	lsquic_conn_ctx_t* _ctx = lsquic_conn_get_ctx(p_conn);
	if (_ctx != nullptr && _ctx->quic != nullptr)
	{
		_ctx->quic->on_connection_closed_sig();
	}
}

static lsquic_stream_ctx* s_on_new_stream(void* unused, struct lsquic_stream* stream)
{
	auto _stream_ctx = (lsquic_stream_ctx*)calloc(1, sizeof(lsquic_stream_ctx));
	if (_stream_ctx == nullptr)
	{
		// error
		return nullptr;
	}
	lsquic_stream_wantread(stream, 1);
	return _stream_ctx;
}

static size_t s_read_and_discard(
	void* p_user_data,
	const unsigned char* p_buf,
	size_t p_count,
	int p_fin)
{
	return p_count;
}

static size_t s_buffer_size(void* p_lsqr_ctx)
{
	auto* _stream_ctx = (lsquic_stream_ctx* const)p_lsqr_ctx;
	if (_stream_ctx == nullptr)
	{
		return 0;
	}
	return std::get<0>(_stream_ctx->u);
}

static size_t s_buffer_read(void* p_lsqr_ctx, void* buf, size_t p_count)
{
	auto* _stream_ctx = (lsquic_stream_ctx* const)p_lsqr_ctx;
	if (_stream_ctx == nullptr)
	{
		return 0;
	}

	auto left = s_buffer_size(_stream_ctx);
	if (p_count > left)
	{
		p_count = left;
	}

	memset(buf, 0, p_count);
	std::get<0>(_stream_ctx->u) -= p_count;
	return p_count;
}

static void s_on_read(lsquic_stream* p_stream, lsquic_stream_ctx* p_stream_ctx)
{
	ssize_t _nr;
	size_t _toread;

	auto _u_left = std::get<0>(p_stream_ctx->u);
	auto _u_buf = std::get<1>(p_stream_ctx->u);

	auto _size_u_left = sizeof(_u_left);
	auto _size_u_buf = sizeof(_u_buf);

	if (p_stream_ctx->n_h_read < _size_u_buf)
	{
		// Read the header
		_toread = _size_u_buf - p_stream_ctx->n_h_read;
		_nr = lsquic_stream_read(
			p_stream,
			&_u_buf[0] + _size_u_buf - _toread,
			_toread);
		if (_nr > 0)
		{
			p_stream_ctx->n_h_read += _nr;
			if (p_stream_ctx->n_h_read == _size_u_left)
			{
#if __BYTE_ORDER == __LITTLE_ENDIAN
				std::get<0>(p_stream_ctx->u) = bswap_64(_u_left);
#endif
				//LSQ_INFO("client requests %"PRIu64" bytes on stream %"PRIu64,
					//stream_ctx->u.left, lsquic_stream_id(stream));
			}
		}
		else if (_nr < 0)
		{
			//LSQ_WARN("error reading from stream: %s", strerror(errno));
			lsquic_stream_close(p_stream);
		}
		else
		{
			//LSQ_WARN("incomplete header on stream %"PRIu64", abort connection",
				//lsquic_stream_id(stream));
			lsquic_stream_wantread(p_stream, 0);
			lsquic_conn_abort(lsquic_stream_conn(p_stream));
		}
	}
	else
	{
		// Read up until FIN, discarding whatever the client is sending
		_nr = lsquic_stream_readf(p_stream, s_read_and_discard, nullptr);
		if (_nr == 0)
		{
			lsquic_stream_wantread(p_stream, 0);
			lsquic_stream_wantwrite(p_stream, 1);
		}
		else if (_nr < 0)
		{
			//LSQ_WARN("error reading from stream: %s", strerror(errno));
			lsquic_stream_close(p_stream);
		}
	}
}

static void s_on_write(lsquic_stream* p_stream, lsquic_stream_ctx* p_stream_ctx)
{
	auto _reader = lsquic_reader
	{
		s_buffer_read, s_buffer_size, p_stream_ctx,
	};
	auto _nw = lsquic_stream_writef(p_stream, &_reader);
	if (_nw >= 0)
	{
		//LSQ_DEBUG("%s: wrote %zd bytes", __func__, nw);
	}
	else
	{
		//LSQ_WARN("%s: cannot write to stream: %s", __func__, strerror(errno));
	}

	if (std::get<0>(p_stream_ctx->u) == 0)
	{
		lsquic_stream_shutdown(p_stream, 1);
	}
}

static void s_on_close(lsquic_stream_t* p_stream, lsquic_stream_ctx_t* p_stream_ctx)
{
	//LSQ_DEBUG("stream closed");
	if (p_stream_ctx)
	{
		free(p_stream_ctx);
	}
}

static const lsquic_stream_if s_stream_callbacks = {
	.on_new_conn = s_on_conn_new,
	.on_conn_closed = s_on_conn_closed,
	.on_new_stream = s_on_new_stream,
	.on_read = s_on_read,
	.on_write = s_on_write,
	.on_close = s_on_close,
};

w_quic::w_quic(w_quic_flags p_flags) :
	_ctx(std::make_unique<ctx>(p_flags))
{
}

w_quic::~w_quic()
{
}

W_RESULT w_quic::init()
{
#ifdef WIN32
	WSADATA wsd;
	int s = WSAStartup(MAKEWORD(2, 2), &wsd);
	if (s != 0)
	{
		//LSQ_ERROR("WSAStartup failed: %d", s);
		return W_RESULT::FAILURE;
	}
#endif

	// initialize sport head
	TAILQ_INIT(&this->_ctx->sports);

	// initialize quic's settings
	lsquic_engine_init_settings(&this->_ctx->settings, this->_ctx->flags);
#if ECN_SUPPORTED
	this->_ctx->settings.es_ecn = LSQUIC_DF_ECN;
#else
	this->_ctx->settings.es_ecn = 0;
#endif

	lsquic_engine_api engine_api = { 0 };

	engine_api.ea_settings = &this->_ctx->settings;
	engine_api.ea_stream_if = &s_stream_callbacks;
	engine_api.ea_stream_if_ctx = this; // use w_quic as stream_if_ctx
	engine_api.ea_packets_out = s_sport_packets_out;
	engine_api.ea_packets_out_ctx = this;
	engine_api.ea_pmi = &pmi;
	engine_api.ea_pmi_ctx = &prog->prog_pba;
	engine_api.ea_get_ssl_ctx = get_ssl_ctx;

//#if LSQUIC_PREFERRED_ADDR
//	if (getenv("LSQUIC_PREFERRED_ADDR4") || getenv("LSQUIC_PREFERRED_ADDR6"))
//		prog->prog_flags |= PROG_SEARCH_ADDRS;
//#endif

	// lookup certificate
	engine_api.ea_lookup_cert = {};//sni_lookup,
	engine_api.ea_cert_lu_ctx = nullptr;
	engine_api.ea_get_ssl_ctx = nullptr;//get_ssl_ctx,
	
	// these are zero anyways
	engine_api.ea_shi = nullptr;
	engine_api.ea_hsi_ctx = 0;
	engine_api.ea_hsi_if = nullptr;//&hset_if,

	this->_ctx->engine = lsquic_engine_new(this->_ctx->flags, &engine_api);

	return W_RESULT::SUCCESS;
}

W_RESULT w_quic::init_global(w_quic_global_flags p_global_flags)
{
	auto _flags = gsl::narrow_cast<int>(p_global_flags);
	if (0 != lsquic_global_init(_flags))
	{
		return W_RESULT::FAILURE;
	}
	return W_RESULT::SUCCESS;
}

W_RESULT w_quic::fini_global()
{
	lsquic_global_cleanup();
	return W_RESULT::SUCCESS;
}



#endif