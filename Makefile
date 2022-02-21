CXX=g++
CXXFLAGS=-lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
BASEDIR=$(PWD)
INCLUDEDIR=$(BASEDIR)/Include
NAME=3DCOM
RUNNER=$(shell whoami)
GDB=gdb

SRC=main.cpp

$(NAME) : $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -I $(INCLUDEDIR) -o $(NAME)

Debug : $(NAME)
	$(CXX) $(SRC) $(CXXFLAGS) -I $(INCLUDEDIR) -g -o $(NAME)
	$(GDB) $(NAME)

prepare :
	@if [ $(RUNNER) != root ] ; \
	then \
		echo "You are not root. Have password ready"; \
		sudo apt install build-essential libglu1-mesa-dev libpng-dev; \
	else \
		echo "You are root" ; \
		apt install build-essential libglu1-mesa-dev libpng-dev; \
	fi

run : $(NAME)
	@sudo ./$(NAME) # To Beep

clean :
	rm -f $(NAME)