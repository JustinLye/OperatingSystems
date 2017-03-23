#include<iostream>
#if !defined(__TP_PID_MANAGER_HEADER__)
#define __TP_PID_MANAGER_HEADER__

#if defined(PIDMGR_LOG_ALL)
#if !defined(PIDMGR_LOG_ALLOC_PID)
#define PIDMGR_LOG_ALLOC_PID
#endif
#if !defined(PIDMGR_LOG_RELEASE_PID)
#define PIDMGR_LOG_RELEASE_PID
#endif
#endif

#if defined __unix__ 
#include<unistd.h>
#include<pthread.h>
#elif defined _WIN32
#include<mutex>
#include<stdexcept>
#if defined(PIDMGR_LOG_RELEASE_PID) || defined(PIDMGR_LOG_ALLOC_PID)
#include<Windows.h>
#endif
#endif

namespace tp {
	enum {
		MIN_PID = 300,
		MAX_PID = 5000
	};

#if defined(__unix__)
	class pthread_locker {
	private:
		pthread_mutex_t *_mux;
		pthread_locker() {}
		pthread_locker(const pthread_locker&) {}
		pthread_locker(pthread_locker&&) {}
	public:
		pthread_locker(pthread_mutex_t *mux) : _mux(mux) {
			pthread_mutex_lock(_mux);
		}
		~pthread_locker() {
			pthread_mutex_unlock(_mux);
		}
	};
#endif

	class pid_manager {
	private:
		int _minpid;
		int _maxpid;
		int _map_upperbound;
		int _last_point;
		int *_pid_map;
#if defined __unix__
		pthread_mutex_t _map_mux;
#elif defined _WIN32
		std::mutex _map_mux;
#endif
		int _search_map(int start_point);
	public:
		pid_manager(int minpid = tp::MIN_PID, int maxpid = tp::MAX_PID);
		~pid_manager();
		int alloc_map();
		int alloc_pid(bool print_when_unavailable = false);
		void release_pid(int pid_num);
	};
};

#endif
