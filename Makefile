.PHONY:all
server=ChatServer
client=ChatClient
INCLUDE=-I./lib/include
LIB_PATH=-L./lib/lib
LDFLAGA=-std=c++11 -lpthread -ljsoncpp -static

all:$(server) $(client)
$(server):ChatServer.cc
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGA)
$(client):ChatClient.cc
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGA)
.PHONY:clean
clean:
	rm -f $(server) $(client)