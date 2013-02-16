CXX=g++
CXXFLAGS= -c -Wall -Werror --std=c++0x -I. -Ilib
LIBS= -lcrypto -lzmq -lboost_system
SOURCES=server_main.cpp  ws_server.cpp ws_session.cpp  ws_session_manager.cpp \
		lib/utilities_chunk_vector.cpp  lib/utilities_random.cpp  lib/utilities_websocket.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=server.tsk

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LIBS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf *o lib/*o server 
