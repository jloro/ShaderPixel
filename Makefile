CC = g++
FLAGS = -Wall -Wextra -Werror -std=c++11
GLFLAGS = -framework CoreVideo -framework OpenGL -framework IOKit -framework AppKit
LIBFLAGS = -lglfw3

EXEC_NAME = test
SRC_FILES = main.cpp \
			Shader.cpp \
			Camera.cpp \

OBJ_FILES = $(SRC_FILES:.cpp=.o)

SRC_PATH = ./src/
OBJ_PATH = ./.obj/
LIB_PATH = ./lib/
INC_PATH = ./inc/ $(LIB_PATH)glfw/include/ $(LIB_PATH)/glad/ $(LIB_PATH)/glm/

LIBS = glfw/build/src

SRCS = $(addprefix $(SRC_PATH),$(SRC_FILES))
OBJS = $(addprefix $(OBJ_PATH),$(OBJ_FILES))
INCS = $(addprefix -I, $(INC_PATH))
LIB = $(addprefix -L$(LIB_PATH),$(LIBS))

.PHONY : all clean fclean re

all : $(EXEC_NAME)

$(EXEC_NAME) : $(OBJS)
	@$(CC) $(FLAGS) $(LIB) $(LIBFLAGS)  $(GLFLAGS) -o $(EXEC_NAME) $(OBJS) $(LIB_PATH)/glad/glad.o
	@echo "$(EXEC_NAME) compiled ✓"

$(OBJ_PATH)%.o : $(SRC_PATH)%.cpp
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(FLAGS) $(INCS) -o $@ -c $<
	@echo "$@ created ✓"

clean :
	@/bin/rm -rf $(OBJ_PATH)
	@echo "Objects cleaned ✓"

fclean : clean
	@/bin/rm -f $(EXEC_NAME)
	@echo "$(EXEC_NAME) deleted ✓"
re :
	$(MAKE) fclean
	$(MAKE) all

