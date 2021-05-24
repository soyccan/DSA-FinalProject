CC = g++
CXXFLAGS += -std=c++17

TARGETS = run

.PHONY: clean

release: CXXFLAGS += -DNDEBUG -O3
debugopt: CXXFLAGS += -g -O3
debug: CXXFLAGS += -g

release: $(TARGETS)
debugopt: $(TARGETS)
debug: $(TARGETS)

mail_class.o: mail_class.h mail_class.cpp

search_mail_functions.o: search_mail_functions.h search_mail_functions.cpp

run.o: run.cpp

query.o: query.h query.cpp

run: mail_class.o search_mail_functions.o run.o query.o

clean:
	rm -rf *.o $(TARGETS)

pack:
	mkdir pack
	cp *.cpp *.h Makefile pack
	zip -r pack pack
	rm -rf pack