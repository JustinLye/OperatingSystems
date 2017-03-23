CCX = g++
CLIBX = ar rsv
CXXFlags += --std=c++11
DFlags += -D PIDMGR_LOG_ALL

all : pid_test.exe

pid_test.exe : pid_manager.lib main.cpp
	$(CCX) $(CXXFlags) $(DFlags) -o pid_test.exe main.cpp pid_manager.lib

pid_manager.lib : pid_manager.o
	$(CLIBX) pid_manager.lib pid_manager.o
pid_manager.o : pid_manager.cpp pid_manager.h
	$(CCX) $(CXXFlags) $(DFlags) -c pid_manager.cpp

clean :
	rm pid_manager.lib pid_manager.o *.out *.exe
