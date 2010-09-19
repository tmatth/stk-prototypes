OBJECTS=ring_mod.o
LIBS=-lstk
CXXFLAGS=-Wall -Werror -Wfatal-errors -Wextra -pedantic -Weffc++ -O2 -D__UNIX_JACK__ -D__LINUX_ALSA__
NAME=ring_mod

$(NAME): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LIBS)

.PHONY: clean
clean:
	rm -rf $(NAME) $(OBJECTS)
