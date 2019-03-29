#include "userlib/libnachos.h"
#include "userlib/syscall.h"

int main(){
    char buff[6];
    if(Create("test",20))
        Exit(-1);
    
    OpenFileId file = Open("test");
    if(Mmap(file,20))
        Exit(-1);
    
    n_printf("Writing in file\n");
    int write_res = Write("hello",6,file);
    n_printf("should be 6 octets => %d\n",write_res);
    
    if(Seek(0,file))
        Exit(-1);
    
    n_printf("Reading in file\n");
    int read_res = Read(buff,6,file);
    n_printf("should be 6 octets => %d\nshould be hello => %s\n",read_res,buff);
    if(Close(file))
        Exit(-1);
    
    Exit(0);

}