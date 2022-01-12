#OBJS specifies which files to compile as part of the project
OBJS = main.cpp ltexture.cpp backgroundtile.cpp navigation.cpp collision.cpp test.cpp goblin.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -IC:\Users\haworthugh\Documents\SDL\SDL2-2.0.18\i686-w64-mingw32\include\SDL2 -IC:\Users\haworthugh\Documents\SDL\SDL2_image-2.0.5\i686-w64-mingw32\include\SDL2 -IC:\Users\haworthugh\Documents\SDL\SDL2_ttf-2.0.15\i686-w64-mingw32\include\SDL2

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LC:\Users\haworthugh\Documents\SDL\SDL2-2.0.18\i686-w64-mingw32\lib -LC:\Users\haworthugh\Documents\SDL\SDL2_image-2.0.5\i686-w64-mingw32\lib -LC:\Users\haworthugh\Documents\SDL\SDL2_ttf-2.0.15\i686-w64-mingw32\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warningsc
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w #-Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = bin/game

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)