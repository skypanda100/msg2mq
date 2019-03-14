//
// Created by root on 3/14/19.
//

#ifndef MSG2MQ_MQ_H
#define MSG2MQ_MQ_H

#include <stdint.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include <stdarg.h>

static int die_on_error(int x, char const *context);

static int die_on_amqp_error(amqp_rpc_reply_t x, char const *context);

static int die(const char *fmt, ...);

int mq_open(const char *host_ptr, const char *port_ptr, const char *usr_ptr, const char *pwd_ptr);

int mq_send(const char *exchange_ptr, const char *routing_key_ptr, const char *json_ptr);

int mq_close();

#endif //MSG2MQ_MQ_H
