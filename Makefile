CC_FLAGS = -Wall -Wextra -std=gnu++14 -Ofast -g
LD_FLAGS = -Wl
LIBS = -lpthread -lboost_system -lboost_program_options

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/, $(notdir $(CPP_FILES:.cpp=.o)))

sender: $(OBJ_FILES)
	g++ $(LD_FLAGS) $(LIBS) -o $@ $^

obj/%.o: src/%.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

clean:
	rm -f sender obj/*

CC_FLAGS += -MMD
-include $(OBJ_FILES:.o=.d)
