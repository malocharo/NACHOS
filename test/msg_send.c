#include "userlib/syscall.h"
#include "userlib/libnachos.h"

int main() {
    char *buff = "THIS IS A TEST M8TE";
    int nb_send;

    if((nb_send = TtySend(buff))){
        n_printf("Tty succesfully send %d byte over %d\n",nb_send,n_strlen(buff));
        return 0;
    }
    n_printf("error while transmitting byte should be 0  byte => %d",nb_send);
    return -1;

}