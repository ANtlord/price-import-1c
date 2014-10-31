CC=g++ -std=c++0x
#CXXFLAGS=$(shell wx-config --cxxflags)
#LIBS=$(shell wx-config --gl-libs --libs)
#MY_LIBS=-L./libs -lJustSound
#AL_LIBS=-lalut -lvorbisfile -logg

PROJECT_FOLDER=$(shell pwd)
TARGET=$(shell basename `pwd`)

SOURCES=$(wildcard *.cpp src/*.cpp)
OBJ_DIR=obj/Release/
vpath %.o $(OBJ_DIR)
OBJECTS=$(addprefix $(OBJ_DIR), $(SOURCES:%.cpp=%.o))

$(OBJ_DIR)%.o: %.cpp
	$(CC) -c -o $@ $<

all: $(SOURCES) $(TARGET)
    
$(OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir obj
	mkdir $@


$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)


clean:
	rm $(OBJECTS)

