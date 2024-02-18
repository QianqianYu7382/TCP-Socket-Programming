CXX = g++

CXXFLAGS = -Wall -g

PLAYER_TARGET = player
RINGMASTER_TARGET = ringmaster

all: $(PLAYER_TARGET) $(RINGMASTER_TARGET)

$(PLAYER_TARGET): player.o function.o potato.o
	$(CXX) $(CXXFLAGS) -o $(PLAYER_TARGET) player.o function.o potato.o

$(RINGMASTER_TARGET): ringmaster.o function.o potato.o
	$(CXX) $(CXXFLAGS) -o $(RINGMASTER_TARGET) ringmaster.o function.o potato.o

function.o: function.cpp function.hpp
	$(CXX) $(CXXFLAGS) -c function.cpp

player.o: player.cpp potato.hpp
	$(CXX) $(CXXFLAGS) -c player.cpp

ringmaster.o: ringmaster.cpp ringmaster.hpp potato.hpp
	$(CXX) $(CXXFLAGS) -c ringmaster.cpp

potato.o: potato.cpp potato.hpp
	$(CXX) $(CXXFLAGS) -c potato.cpp

clean:
	rm -f $(PLAYER_TARGET) $(RINGMASTER_TARGET) *.o
