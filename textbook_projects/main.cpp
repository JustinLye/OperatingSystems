#if defined(__unix__)
#include<time.h>
#elif defined(_WIN32)
#include<Windows.h>
#include<mutex>
#endif

#include"pid_manager.h"
#define THREAD_COUNT 50


tp::pid_manager pm(200, 220);

#if defined(__unix__)
void *process_id_proxy();
//assumes srand is initialized
struct timespec random_wait_time(time_t min_secs = 0, time_t max_secs = 4, long min_nano = 100, long max_nano = 9999999);
#elif defined(_WIN32)
//assumes srand is initialized
DWORD random_wait_time(DWORD min_time = 50, DWORD max_time = 12000);
DWORD WINAPI process_id_proxy(LPVOID Param);
std::mutex msg_mux;
#endif



int main(int argc, char* argv[]) {
#if defined(__unix__)
#elif defined(_WIN32)
	try {
		DWORD thread_ids[THREAD_COUNT];
		HANDLE thread_handles[THREAD_COUNT];
		DWORD wait_times[THREAD_COUNT];
		srand(1420);
		for (int i = 0; i < THREAD_COUNT; i++) {
			wait_times[i] = random_wait_time();
		}
		pm.alloc_map();
		for (int i = 0; i < THREAD_COUNT; i++) {
			thread_handles[i] = CreateThread(
				NULL,
				0,
				process_id_proxy,
				&wait_times[i],
				0,
				&thread_ids[i]);
		}

		WaitForMultipleObjects(
			THREAD_COUNT,
			thread_handles,
			TRUE,
			INFINITE);
		for (int i = 0; i < THREAD_COUNT; i++) {
			CloseHandle(thread_handles[i]);
		}
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	} catch (...) {
		std::cerr << "error occurred." << std::endl;
	}
#endif
	return 0;
}


#if defined(__unix__)
struct timespec random_wait_time(time_t min_secs, time_t max_secs, long min_nano, long max_nano) {
	timespec tspec;
	tspec.tv_sec = (time_t)((rand() & (max_secs - min_secs)) + min_secs);
	tspec.tv_nsec = (long)((rand() & (max_nano - min_nano)) + min_nano);
	return tspec;
}
void *process_id_proxy() {
	int pid_id = pm.alloc_pid();
	timespec randomsleep = random_wait_time();
	nanosleep(&randomsleep, NULL);
	pm.release_pid(pid_id);
}
#elif defined(_WIN32)
//assuming srand is initialized
DWORD random_wait_time(DWORD min_time, DWORD max_time) {
	return (rand() % (max_time - min_time) + min_time);
}
DWORD WINAPI process_id_proxy(LPVOID Param) {
	int pid_id = pm.alloc_pid(true);
	DWORD tid = GetCurrentThreadId();
	while (pid_id == 1) {
		Sleep(100);
		pid_id = pm.alloc_pid();
	}
#if !defined(PIDMGR_LOG_ALLOC_PID)
	{
		std::lock_guard<std::mutex> msg_guard(msg_mux);
		std::cout << "Thread: " << tid << " was allocated process id " << pid_id << std::endl;
	}
#endif
	Sleep(*(DWORD*)Param);
	pm.release_pid(pid_id);
#if !defined(PIDMGR_LOG_RELEASE_PID)
	{
		std::lock_guard<std::mutex> msg_guard(msg_mux);
		std::cout << "Thread: " << tid << " released process id " << pid_id << std::endl;
	}
#endif
	return 0;
}
#endif

