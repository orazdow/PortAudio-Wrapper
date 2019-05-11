TARGET = out
SRC = main.cpp pa.cpp

INCLUDE = -I ../libraries/portaudio/include 
LIBS = -lportaudio

# on mac/linux: libs are usually installed in /usr/lib which is likely in PATH
LIBS += -L ../libraries/portaudio/build 

CXX = g++
FLAGS = -std=c++11

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(INCLUDE) $(FLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

run:
	@./$(TARGET)
