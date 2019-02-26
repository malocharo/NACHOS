#include "userlib/syscall.h"
#include "userlib/libnachos.h"

#define BUFF_SIZE 256
int main() {
    char msg[BUFF_SIZE];
    int nb_rec;

    if((nb_rec = TtyReceive(msg,BUFF_SIZE))) {
        n_printf("Receive %s size %d\n",msg,nb_rec);
        return 0;
    }
    n_printf("size should be zero and buff empty buff =>%s size => %d\n",msg,nb_rec);
    return -1;

}