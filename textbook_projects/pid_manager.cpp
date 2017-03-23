#include "pid_manager.h"


namespace tp {
	const char* MSG_PID_UNAVAILABLE = "All process id's are currently unavailable.";
	const char* MSG_PID_ID = "Process id ";
	const char* MSG_PID_ALLOC = " has been allocated.";
	const char* MSG_PID_RELEASE = " has been released.";
	const char* MSG_PID_FAILED_RELEASE = " failed to be released. The id may be invalid or the map has not been initialized.";
};

tp::pid_manager::pid_manager(int minpid, int maxpid) :
	_minpid(minpid),
	_maxpid(maxpid),
	_map_upperbound(maxpid - minpid + 1),
	_last_point(0),
	_pid_map(nullptr) {
#if defined(__unix__)
	pthread_mutex_init(&_map_mux, NULL);
#endif
}

tp::pid_manager::~pid_manager() {
	if(_pid_map != nullptr)
		delete[] _pid_map;
}

//finds avaliable pid id if any, returns -1 in no pid id is available
int tp::pid_manager::_search_map(int start_point) {
	
	if (start_point < 0 || start_point >= _map_upperbound) {
		perror("search start_point was not within acceptable bounds.");
		return -1;
	}
	int currpoint = start_point;
	//search to the end of the map
/*
This is unnessary and will probably cause problems. The _map_mux is locked in the only function that calls _search_map.
#if defined(__unix__)
	tp::pthread_locker map_guard(&_map_mux); //lock _pid_map while searching for available id
#elif defined(_WIN32)
	std::lock_guard<std::mutex> map_guard(_map_mux); //lock _pid_map while searching for available id
#endif*/
	while (currpoint < _map_upperbound) {
		if (_pid_map[currpoint] == 0)
			return currpoint;
		else
			currpoint++;
	}
	currpoint = 0;
	//search from beginning to given start point
	while (currpoint < start_point) {
		if (_pid_map[currpoint] == 0)
			return currpoint;
		else
			currpoint++;
	}
	return -1;
}

//initializes bitmap
int tp::pid_manager::alloc_map() {
#if defined(__unix__)
	tp::pthread_locker map_guard(&_map_mux); //lock _pid_map while initializing map
#elif defined(_WIN32)
	std::lock_guard<std::mutex> map_guard(_map_mux); //lock _pid_map while initializing map
#endif
	if(_pid_map != nullptr)
		delete[] _pid_map;
	_pid_map = new int[_map_upperbound];
	for (int i = 0; i < _map_upperbound; i++)
		_pid_map[i] = 0;
	return 1;
}

int tp::pid_manager::alloc_pid(bool print_when_unavailable) {
	if (_pid_map == nullptr) {
		throw std::exception("pid map must be allocated.");
}
#if defined(__unix__)
	tp::pthread_locker map_guard(&_map_mux); //lock _pid_map while allocating id
#elif defined(_WIN32)
	std::lock_guard<std::mutex> map_guard(_map_mux); //lock _pid_map while allocating id
#endif
	int rtn_pid = _search_map(_last_point);
	if (rtn_pid == -1) {
#if defined(PIDMGR_LOG_ALLOC_PID)
		if(print_when_unavailable)
#if defined(_WIN32)
			std::cout << "Thread ID " << GetCurrentThreadId() << " is waiting. "  << std::endl;
#else
			std::cout << tp::MSG_PID_UNAVAILABLE << std::endl;
#endif
#endif
		return 1;
	}
	_pid_map[rtn_pid] = 1;
	_last_point = rtn_pid;
#if defined(PIDMGR_LOG_ALLOC_PID)
	rtn_pid += _minpid;
#if defined(_WIN32)
	std::cout << "Thread ID " << GetCurrentThreadId() << " was allocated process id " << rtn_pid << std::endl;
#else
	std::cout << tp::MSG_PID_ID << rtn_pid << tp::MSG_PID_ALLOC << std::endl;
#endif
	return rtn_pid;
#else
	return rtn_pid + _minpid;
#endif
}

void tp::pid_manager::release_pid(int pid_num) {
	pid_num -= _minpid;
#if defined(__unix__)
	tp::pthread_locker map_guard(&_map_mux); //lock _pid_map while releasing id
#elif defined(_WIN32)
	std::lock_guard<std::mutex> map_guard(_map_mux); //lock _pid_map while releasing id
#endif
	if (pid_num >= 0 && pid_num < _map_upperbound && _pid_map != nullptr) {
		_pid_map[pid_num] = 0;
#if defined(PIDMGR_LOG_RELEASE_PID)
#if defined(_WIN32)
		std::cout << "Thread ID " << GetCurrentThreadId() << " has released process id " << pid_num + _minpid << std::endl;
#else
		std::cout << tp::MSG_PID_ID << pid_num + _minpid << tp::MSG_PID_RELEASE << std::endl;
#endif
#endif
	} else {
#if defined(PIDMGR_LOG_RELEASE_PID)
		std::cout << tp::MSG_PID_ID << pid_num + _minpid << tp::MSG_PID_FAILED_RELEASE << std::endl;
#endif
	}

}