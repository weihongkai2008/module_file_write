#include <unistd.h>
#include <sys/syscall.h>

int main(void){
	syscall(223);
	return 0;
}
