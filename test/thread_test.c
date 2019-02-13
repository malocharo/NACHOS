#include "userlib/libnachos.h"
#include "userlib/syscall.h"

VoidNoArgFunctionPtr func() {
    n_printf("test\n");
    
}

int main() {
	
    threadCreate("test",func);
    return 0;
}
