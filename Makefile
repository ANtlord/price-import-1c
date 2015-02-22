CC=g++ -std=c++0x
#CXXFLAGS=$(shell wx-config --cxxflags)
#LIBS=$(shell wx-config --gl-libs --libs)
VENDOR_LIBS=-L./vendor/forge -lforge -lpqxx -L./vendor/libxls/libxls/build/lib -lxlsreader
VENDOR_HEADERS=-I./vendor/forge/include -I./vendor/libxls/libxls/build/include
#AL_LIBS=-lalut -lvorbisfile -logg

PROJECT_FOLDER=$(shell pwd)
TARGET=$(shell basename `pwd`)

SOURCES=$(wildcard *.cpp src/*.cpp)
OBJ_DIR=obj/Release/
vpath %.o $(OBJ_DIR)
OBJECTS=$(addprefix $(OBJ_DIR), $(SOURCES:%.cpp=%.o))

$(OBJ_DIR)%.o: %.cpp
	$(CC) -c -o $@ $< $(VENDOR_LIBS) $(VENDOR_HEADERS)

all: $(SOURCES) $(TARGET)

debug:
	$(CC) -g $(SOURCES) $(VENDOR_LIBS) $(VENDOR_HEADERS)
   
$(OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir obj
	mkdir $@


$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(VENDOR_LIBS) $(VENDOR_HEADERS)


clean:
	rm $(OBJECTS)
