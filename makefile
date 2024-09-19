#ngl i never learned makefile

CC = g++
CFLAGS  = -lglfw -lGL -lGLEW -lX11 -lpthread -lXrandr -lXi -ldl #magic shit that makes compiler happy :)
 
TARGET = main
 
all: $(TARGET)
 
$(TARGET): $(TARGET).cpp
	$(CC) -o $(TARGET) $(TARGET).cpp $(CFLAGS) 
	./$(TARGET)
 
clean:
	$(RM) $(TARGET)