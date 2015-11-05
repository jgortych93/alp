#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

char* (*hello_wsk)();

int main(){

	void* uchwyt=dlopen("./libwsp.so", RTLD_LAZY);
	if(!uchwyt){
		dlerror();
		return 1;
	}

	hello_wsk=dlsym(uchwyt,"hello");
	char* czesc=hello_wsk();
	printf("!!!%s\n",czesc);
	
	



return 0;
}
