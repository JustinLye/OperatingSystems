
#include"pid_manager.h"


int main(int argc, char* argv[]) {
	tp::pid_manager pm(300, 303);
	pm.alloc_map();
	pm.alloc_pid();
	int a = pm.alloc_pid();
	pm.alloc_pid();
	pm.release_pid(a);
	pm.alloc_pid();
	pm.alloc_pid();
	pm.release_pid(a);
	pm.alloc_pid();
	return 0;
}