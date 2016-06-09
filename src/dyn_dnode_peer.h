/*
 * Dynomite - A thin, distributed replication layer for multi non-distributed storages.
 * Copyright (C) 2014 Netflix, Inc.
 */ 
#include "dyn_core.h"
#include "dyn_server.h"


#ifndef _DYN_DNODE_PEER_H_
#define _DYN_DNODE_PEER_H_

#define WAIT_BEFORE_RECONNECT_IN_MILLIS      30000
#define WAIT_BEFORE_UPDATE_PEERS_IN_MILLIS   30000

struct peer {
    uint32_t           idx;           /* server index */
    struct server_pool *owner;        /* owner pool */
    struct endpoint     endpoint;
    struct string      name;          /* name (ref in conf_server) */
    struct conn        *conn;         /* active connection to peer */

    msec_t             next_retry;    /* next retry time in msec */
    uint32_t           failure_count; /* # consecutive failures */

    struct string      rack;          /* logical rack */
    struct string      dc;            /* server's dc */
    struct array       tokens;        /* DHT tokens this peer owns */
    bool               is_local;      /* is this peer the current running node?  */
    unsigned           is_seed:1;     /* seed? */
    unsigned           processed:1;   /* flag to indicate whether this has been processed */
    unsigned           is_secure:1;   /* is the connection to the server secure? */
    dyn_state_t        state;         /* state of the server - used mainly in peers  */
};

msec_t dnode_peer_timeout(struct msg *msg, struct conn *conn);
rstatus_t dnode_peer_init(struct context *ctx);
void dnode_peer_connected(struct context *ctx, struct conn *conn);
void dnode_peer_deinit(struct array *nodes);
rstatus_t dnode_peer_add_local(struct server_pool *pool, struct peer *self);
rstatus_t dnode_peer_each_set_evb(void *elem, void *data);
rstatus_t dnode_peer_each_set_owner(void *elem, void *data);

struct peer *get_dnode_peer_in_rack_for_key(struct context *ctx, struct server_pool *pool,
                               struct rack *rack, uint8_t *key, uint32_t keylen,
                               uint8_t msg_type);
void dnode_req_forward_error(struct context *ctx, struct conn *p_conn, struct msg *msg, err_t error);
void dnode_peer_req_forward(struct context *ctx, struct conn *c_conn, struct peer *peer,
		                    struct msg *msg, uint8_t *key, uint32_t keylen);
rstatus_t dnode_peer_pool_preconnect(struct context *ctx);
void dnode_peer_pool_disconnect(struct context *ctx);
rstatus_t dnode_peer_forward_state(void *rmsg);
rstatus_t dnode_peer_add(void *rmsg);
rstatus_t dnode_peer_replace(void *rmsg);
rstatus_t dnode_peer_handshake_announcing(void *rmsg);

void init_dnode_peer_conn(struct conn *conn);
void preselect_remote_rack_for_replication(struct context *ctx);
#endif 
