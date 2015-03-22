CXX=g++ -std=c++0x
COV_KEYS=-g -O0 --coverage
#CXXFLAGS=$(shell wx-config --cxxflags)
#LIBS=$(shell wx-config --gl-libs --libs)
PROJECT_FOLDER=$(shell pwd)
VENDOR_LIBS=-L./vendor/forge -lforge -lpqxx -L./vendor/libxls/libxls/build/lib -lxlsreader
VENDOR_HEADERS=-I$(PROJECT_FOLDER)/vendor/forge/include -I$(PROJECT_FOLDER)/vendor/libxls/libxls/build/include -I./vendor/cxxtest/
#AL_LIBS=-lalut -lvorbisfile -logg

export LD_LIBRARY_PATH=$(PROJECT_FOLDER)/vendor/libxls/libxls/build/lib
TARGET=$(shell basename `pwd`)

SOURCES=$(wildcard *.cpp src/*.cpp)
TEST_SOURCES=$(wildcard src/*.cpp)

OBJ_DIR=obj/Release/
vpath %.o $(OBJ_DIR)
OBJECTS=$(addprefix $(OBJ_DIR), $(SOURCES:%.cpp=%.o))
COVOBJECTS=$(addprefix $(OBJ_DIR), $(SOURCES:%.cpp=%.gcno))
GCDA_OBJECTS=$(addprefix $(OBJ_DIR), $(SOURCES:%.cpp=%.gcda))
GCOV_FILES=$(wildcard *.gcov)
TEST_OBJECTS=$(addprefix $(OBJ_DIR), $(TEST_SOURCES:%.cpp=%.o))
UNIT_FILES=unittests/*.h
RUNNER=runner
RUNNER_CPP=unittests/runner.cpp

$(OBJ_DIR)%.o: %.cpp
	$(CXX) -c -o $@ $< $(VENDOR_LIBS) $(VENDOR_HEADERS)

all: $(SOURCES) $(TARGET)

debug:
	$(CXX) -g $(SOURCES) $(VENDOR_LIBS) $(VENDOR_HEADERS)
   
$(OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $@/src

$(TARGET): $(OBJECTS)
	$(CXX) -o main $(OBJECTS) $(VENDOR_LIBS) $(VENDOR_HEADERS)

clean:
	rm $(COVOBJECTS); rm $(OBJECTS); rm $(GCDA_OBJECTS); rm $(RUNNER); rm $(RUNNER_CPP); rm $(GCOV_FILES)

# unit tests.
$(RUNNER_CPP):
	./vendor/cxxtest/bin/cxxtestgen --error-printer -o $(RUNNER_CPP) $(UNIT_FILES)

$(RUNNER): $(TEST_OBJECTS) $(RUNNER_CPP)
	$(CXX) -o $(RUNNER) $(RUNNER_CPP) $(TEST_OBJECTS) $(VENDOR_HEADERS) $(VENDOR_LIBS)

test: $(RUNNER)
	./$(RUNNER)

# coverage.
coverage: CXX=g++ -std=c++0x -O0 --coverage -g
coverage: $(RUNNER)
	./$(RUNNER)
	coveralls --gcov-options='-r -o $(OBJ_DIR)src/ $(TEST_SOURCES)' --gcov=/usr/bin/gcov-4.8
