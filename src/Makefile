CC = g++
CXXFLAGS += -std=c++17 -O3

TARGETS = run

.PHONY: clean

all: $(TARGETS)

mail_class.o: mail_class.h mail_class.cpp

search_mail_functions.o: search_mail_functions.h search_mail_functions.cpp

run.o: run.cpp

run: mail_class.o search_mail_functions.o run.o

clean:
	rm -rf *.o $(TARGETS) 
