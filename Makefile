CXX=g++
CXXFLAGS=-lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
BASEDIR=$(PWD)
INCLUDEDIR=$(BASEDIR)/Include
NAME=3DCOM
RUNNER=$(shell whoami)

SRC=main.cpp

$(NAME) : $(SRC)
	$(CXX)  $(CXXFLAGS) -I $(INCLUDEDIR) -o $(NAME) $(SRC)

prepare :
	@if [ $(RUNNER) != root ] ; \
	then \
		echo "You are not root. Have password ready"; \
		sudo apt install build-essential libglu1-mesa-dev libpng-dev; \
	else \
		echo "You are root" ; \
		apt install build-essential libglu1-mesa-dev libpng-dev; \
	fi