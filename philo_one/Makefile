NAME = philo_one

SRC_DIR = ./src/
SRC_FILES = main.c utils.c put_status_msg.c initialize_variables.c phi_t.c

OBJ_DIR = ./obj/
OBJ_FILES = $(addprefix $(OBJ_DIR), $(SRC_FILES:.c=.o))

# INCLUDE_DIR = ./
INCLUDE_FILES = philo_one.h

CC = gcc
FLAGS = -Wall -Wextra -Werror

RM = /bin/rm -f
RMDIR = /bin/rm -rf

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ_FILES) $(INCLUDE_FILES)
	$(CC) $(FLAGS) -I$(INCLUDE_FILES) $(OBJ_FILES) -o $(NAME)

# make OBJ_DIR for NAME dependency
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

# make .o files for NAME dependency
$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	gcc -I$(INCLUDE_FILES) -c -o $@ $<

clean:
	$(RM) $(OBJ)
veryclean: clean
	$(RMDIR) $(OBJ_DIR)
fclean: clean
	$(RM) $(NAME)
re: fclean all
.PHONY : all clean fclean re