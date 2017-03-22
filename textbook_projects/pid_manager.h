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

namespace tp {
	enum {
		MIN_PID = 300,
		MAX_PID = 5000
	};
	
	class pid_manager {
	private:
		int _minpid;
		int _maxpid;
		int _map_upperbound;
		int _last_point;
		int *_pid_map;
		int _search_map(int start_point);
	public:
		pid_manager(int minpid = tp::MIN_PID, int maxpid = tp::MAX_PID);
		~pid_manager();
		int alloc_map();
		int alloc_pid();
		void release_pid(int pid_num);
	};
};

#endif
