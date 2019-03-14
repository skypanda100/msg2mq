//
// Created by root on 3/14/19.
//

#include "mq.h"
#include <stdlib.h>
#include <stdio.h>

static amqp_connection_state_t conn;
static amqp_socket_t *socket = NULL;

static int die_on_error(int x, char const *context)
{
    if(x < 0)
    {
        fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x));
        return 1;
    }
    return 0;
}

static int die_on_amqp_error(amqp_rpc_reply_t x, char const *context)
{
    switch (x.reply_type) {
        case AMQP_RESPONSE_NORMAL:
            return 0;

        case AMQP_RESPONSE_NONE:
            fprintf(stderr, "%s: missing RPC reply type!\n", context);
            break;

        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x.library_error));
            break;

        case AMQP_RESPONSE_SERVER_EXCEPTION:
            switch(x.reply.id)
            {
                case AMQP_CONNECTION_CLOSE_METHOD:
                {
                    amqp_connection_close_t *m = (amqp_connection_close_t *) x.reply.decoded;
                    fprintf(stderr, "%s: server connection error %uh, message: %.*s\n",
                            context,
                            m->reply_code,
                            (int) m->reply_text.len, (char *) m->reply_text.bytes);
                    break;
                }
                case AMQP_CHANNEL_CLOSE_METHOD:
                {
                    amqp_channel_close_t *m = (amqp_channel_close_t *) x.reply.decoded;
                    fprintf(stderr, "%s: server channel error %uh, message: %.*s\n",
                            context,
                            m->reply_code,
                            (int) m->reply_text.len, (char *) m->reply_text.bytes);
                    break;
                }
                default:
                    fprintf(stderr, "%s: unknown server error, method id 0x%08X\n", context, x.reply.id);
                    break;
            }
            break;
    }

    return -1;
}

static int die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");

    return -1;
}

int mq_open(const char *host_ptr, const char *port_ptr, const char *usr_ptr, const char *pwd_ptr)
{
    int status = 0;

    conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);
    if(!socket)
    {
        die("Creating TCP socket failed!");
        return 1;
    }

    status = amqp_socket_open(socket, host_ptr, atoi(port_ptr));
    if(status != 0)
    {
        die("Opening TCP socket failed!");
        return -1;
    }

    status = die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, usr_ptr, pwd_ptr),
                      "Logging in");
    if(status != 0)
    {
        return -1;
    }

    amqp_channel_open(conn, 1);
    status = die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");
    if(status != 0)
    {
        return -1;
    }

    return 0;
}

int mq_send(const char *exchange_ptr, const char *routing_key_ptr, const char *json_ptr)
{
    amqp_basic_properties_t props;

    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("application/json");

    props.delivery_mode = 2; /* persistent delivery mode */

    return die_on_error(amqp_basic_publish(conn,
                                           1,
                                           amqp_cstring_bytes(exchange_ptr),
                                           amqp_cstring_bytes(routing_key_ptr),
                                           0,
                                           0,
                                           &props,
                                           amqp_cstring_bytes(json_ptr)),
                        "Publishing");
}

int mq_close()
{
    if(die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS), "Closing channel") != 0)
    {
        return -1;
    }
    if(die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS), "Closing connection") != 0)
    {
        return -1;
    }
    if(die_on_error(amqp_destroy_connection(conn), "Ending connection") != 0)
    {
        return -1;
    }

    return 0;
}
