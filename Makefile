#CXX = g++
#CXXFLAGS = -std=c++11 -Wall

scheduler: Scheduler.cpp
	 g++ --std=c++11 Scheduler.cpp -o scheduler

clean:
	rm -f scheduler *~