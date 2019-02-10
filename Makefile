TARGET := out.exe
SRC := main.cpp pa.cpp

INCLUDE = -ID:/libraries/portaudio/include 
LIBS = -LD:/libraries/portaudio/build 
LIBS += -lportaudio

CXX := g++
# FLAGS := -Wall

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(INCLUDE) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(TARGET)

run:
	@./$(TARGET)
