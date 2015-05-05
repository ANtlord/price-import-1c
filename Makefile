CXX=g++ -std=c++11
#CXXFLAGS=$(shell wx-config --cxxflags)
#LIBS=$(shell wx-config --gl-libs --libs)
PROJECT_FOLDER=$(shell pwd)
VENDOR_LIBS=-L./vendor/forge -lforge -lpqxx -L./vendor/libxls/libxls/build/lib -lxlsreader
VENDOR_HEADERS=-I$(PROJECT_FOLDER)/vendor/forge/include -I$(PROJECT_FOLDER)/vendor/libxls/libxls/build/include \
			   -I$(PROJECT_FOLDER)/vendor/cxxtest/ -I$(PROJECT_FOLDER)/vendor/el/ \
			   -I$(PROJECT_FOLDER)/vendor/tclap-code/build/include/
#AL_LIBS=-lalut -lvorbisfile -logg

export LD_LIBRARY_PATH=$(PROJECT_FOLDER)/vendor/libxls/libxls/build/lib
TARGET=$(shell basename `pwd`)

SOURCES=$(wildcard *.cpp src/*.cpp)
TEST_SOURCES=$(wildcard src/*.cpp)
UNIT_FILES=$(wildcard unittests/*.h)

OBJ_DIR=obj/Release/
vpath %.o $(OBJ_DIR)
OBJECTS=$(addprefix $(OBJ_DIR), $(SOURCES:%.cpp=%.o))
COVOBJECTS=$(addprefix $(OBJ_DIR), $(SOURCES:%.cpp=%.gcno))
GCDA_OBJECTS=$(addprefix $(OBJ_DIR), $(SOURCES:%.cpp=%.gcda))
GCOV_FILES=$(wildcard *.gcov)
TEST_OBJECTS=$(addprefix $(OBJ_DIR), $(TEST_SOURCES:%.cpp=%.o))
RUNNER=runner
RUNNER_CPP=unittests/runner.cpp

$(OBJ_DIR)%.o: %.cpp
	$(CXX) -c -o $@ $< $(VENDOR_LIBS) $(VENDOR_HEADERS)

all: $(SOURCES) $(TARGET)

debug: CXX=g++ -std=c++0x -g
debug: $(SOURCES) $(TARGET)

release: CXX=g++ -std=c++0x -O3 -march=native
release: $(SOURCES) $(TARGET)
   
$(OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $@/src

$(TARGET): $(OBJECTS)
	$(CXX) -o main $(OBJECTS) $(VENDOR_LIBS) $(VENDOR_HEADERS)

clean:
	rm $(COVOBJECTS); rm $(OBJECTS); rm $(GCDA_OBJECTS); rm $(RUNNER); rm $(RUNNER_CPP); rm $(GCOV_FILES)

# unit tests.
$(RUNNER_CPP): $(UNIT_FILES)
	$(PROJECT_FOLDER)/vendor/cxxtest/bin/cxxtestgen --error-printer \
		--template=$(PROJECT_FOLDER)/unittests/runner.tpl -o $(RUNNER_CPP) $(UNIT_FILES)

$(RUNNER): $(TEST_OBJECTS) $(RUNNER_CPP)
	$(CXX) -o $(RUNNER) $(RUNNER_CPP) $(TEST_OBJECTS) $(VENDOR_HEADERS) $(VENDOR_LIBS)

test: $(RUNNER)
	./$(RUNNER)

# coverage.
cov: CXX=g++ -std=c++0x -O0 --coverage -g
cov: $(RUNNER)
	./$(RUNNER)
	gcov -r -o $(OBJ_DIR)src/ $(TEST_SOURCES) 

# Only for travis environment
coveralls: CXX=g++ -std=c++0x -O0 --coverage -g
coveralls: $(RUNNER)
	./$(RUNNER)
	coveralls --gcov-options='-r -o $(OBJ_DIR)src/ $(TEST_SOURCES)' --gcov=/usr/bin/gcov-4.8
