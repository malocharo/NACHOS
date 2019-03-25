#include "userlib/syscall.h"
#include "userlib/libnachos.h"

int main() {
    char *buff = "hello world!";
    int nb_send;

    if((nb_send = TtySend(buff))){
        n_printf("Tty succesfully send %s => %d byte over %d\n",buff,nb_send,n_strlen(buff)+1);
        return 0;
    }
    n_printf("error while transmitting byte should be 0  byte => %d",nb_send);
    return -1;

}