CC := g++
CXXFLAGS := -std=gnu++17

TARGETS = run

.PHONY: clean

release: CXXFLAGS += -DNDEBUG -O3 -g
debugopt: CXXFLAGS += -g -O3
debug: CXXFLAGS += -g

release: $(TARGETS)
debugopt: $(TARGETS)
debug: $(TARGETS)

mail_class.o: mail_class.h mail_class.cpp

run.o: run.cpp

run: mail_class.o run.o

clean:
	rm -rf *.o $(TARGETS)

pack:
	mkdir pack
	cp *.cpp *.h Makefile pack
	zip -r pack pack
	rm -rf pack
