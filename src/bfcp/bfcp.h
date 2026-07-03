/**
 * @file bfcp.h Internal interface to Binary Floor Control Protocol (BFCP)
 *
 * Copyright (C) 2010 Creytiv.com
 */

/*
 * Front headroom reserved in front of every outgoing BFCP message. It must be
 * large enough for the biggest wrapper that a lower layer may prepend without
 * reallocating the mbuf. The TURN client (re/src/turn/turnc.c) wraps outgoing
 * packets either in a 4-byte ChannelData header or, when no channel binding
 * exists for the destination, in a STUN Send indication (up to 48 bytes for
 * IPv6: STUN header 20 + 2*attr header 8 + XOR-PEER-ADDR 20). Reserving only
 * the ChannelData size would make the send-indication path silently fail and
 * leak the raw BFCP packet straight to the peer, bypassing the relay.
 */
enum {
	BFCP_HDR_OFFSET = 48,
};

struct bfcp_strans {
	enum bfcp_prim prim;
	uint32_t confid;
	uint16_t tid;
	uint16_t userid;
};

struct bfcp_conn {
	struct bfcp_strans st;
	struct list ctransl;
	struct tmr tmr1;
	struct tmr tmr2;
	struct udp_sock *us;
	struct tcp_sock *ts;
	struct tcp_conn *tc;
	struct sa sa_peer;
	struct mbuf *mb;
	bfcp_conn_h *connh;
	bfcp_estab_h *estabh;
	bfcp_recv_h *recvh;
	bfcp_close_h *closeh;
	void *arg;
	enum bfcp_transp tp;
	unsigned txc;
	uint16_t tid;
};


/* attributes */
int bfcp_attrs_decode(struct list *attrl, struct mbuf *mb, size_t len,
		      struct bfcp_unknown_attr *uma);
struct bfcp_attr *bfcp_attrs_find(const struct list *attrl,
				  enum bfcp_attrib type);
struct bfcp_attr *bfcp_attrs_apply(const struct list *attrl,
				   bfcp_attr_h *h, void *arg);
int bfcp_attrs_print(struct re_printf *pf, const struct list *attrl,
		     unsigned level);


/* connection */
int bfcp_send(struct bfcp_conn *bc, const struct sa *dst, struct mbuf *mb);


/* request */
bool bfcp_handle_response(struct bfcp_conn *bc, const struct bfcp_msg *msg);
int  bfcp_vrequest(struct bfcp_conn *bc, const struct sa *dst, uint8_t ver,
		   enum bfcp_prim prim, uint32_t confid, uint16_t userid,
		   bfcp_resp_h *resph, void *arg, unsigned attrc, va_list *ap);
