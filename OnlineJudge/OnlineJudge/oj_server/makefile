oj_server:oj_server.cc
	g++ -o $@ $^ -I./include -L./lib -std=c++17 -lpthread -lctemplate -ljsoncpp -lmysqlclient
.PHONY:clean
clean:
	rm -f oj_server