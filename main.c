#include <stdio.h>
#include "mq.h"

int main(int argc,char **argv)
{
    // init
    if(argc != 8)
    {
        fprintf(stderr, "please input arguments like this: user, password, host, port, exchange, routing key, json\n");

        return -1;
    }
    char *usr_ptr = argv[1];
    char *pwd_ptr = argv[2];
    char *host_ptr = argv[3];
    char *port_ptr = argv[4];
    char *exchange_ptr = argv[5];
    char *routing_key_ptr = argv[6];
    char *json_ptr = argv[7];

    printf("input arguments:\nuser=%s\npassword=%s\nhost=%s\nport=%s\nexchange=%s\nroutingKey=%s\njson=%s\n",
           usr_ptr, pwd_ptr, host_ptr, port_ptr,
           exchange_ptr, routing_key_ptr, json_ptr);

    // open mq
    if(mq_open(host_ptr, port_ptr, usr_ptr, pwd_ptr) != 0)
    {
        return -1;
    }

    // send message to mq
    if(mq_send(exchange_ptr, routing_key_ptr, json_ptr) != 0)
    {
        goto END;
    }

END:
    // close mq
    if(mq_close() != 0)
    {
        return -1;
    }

    return 0;
}