FLAG =-std=c++11 -lboost_filesystem -lboost_system \
	-I /home/ferlan/search_engine/cppjieba/include -ljsoncpp 

.PHONY:all 
all: test libsearcher.so

test:test.cpp searcher.h searcher.cpp 
	g++ test.cpp searcher.cpp -g -o test $(FLAG)


libsearcher.so:searcher.cpp searcher.h 
	g++ searcher.cpp -shared -fPIC -o libsearcher.so $(FLAG)


http_server:http_server.cpp libsearcher.so                                                              
	g++ http_server.cpp libsearcher.so -o http_sever -std=c++11 -lpthread

