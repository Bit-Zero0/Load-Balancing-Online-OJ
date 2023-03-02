oj_server:oj_server.cpp
	g++ -o $@ $^ -I./include -L./lib  -std=c++11 -lpthread -lctemplate -ljsoncpp -lmysqlclient

.PHONY:clean
clean:
	rm -f oj_server