CXX=g++
CXXFLAGS= -c -Wall -Werror -std=c++11 -pedantic -I. -Ilib -g
LIBS= -lcrypto -lzmq -lboost_system
SOURCES=server_main.cpp  ws_server.cpp ws_session.cpp  ws_session_manager.cpp ws_dealer.cpp zmq_dealer.cpp\
		lib/utilities_chunk_vector.cpp  lib/utilities_random.cpp  lib/utilities_websocket.cpp lib/utilities_debugging.cpp lib/utilities_print.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=server.tsk

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LIBS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf *o lib/*o server.tsk
