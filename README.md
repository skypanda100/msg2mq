# msg2mq
msg2mq [user] [pwd] [host] [port] [exchange] [routing key] [context]

# Example Usage
*project path is `/root/msg2mq`*
* build  
    ```bash
    $ cd /root/msg2mq
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    ```
* execute  
    ```bash
    $ cd /root/msg2mq
    $ ./msg2mq guest guest 127.0.0.1 5672 amq.topic test.a "{\"key\":\"value\"}"
    ```
