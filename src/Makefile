CC := g++
CXXFLAGS := -std=gnu++17 -Ismhasher/src -msse4.2 -Icityhash/src
LDFLAGS := -Lcityhash/src/.libs -lcityhash

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

testhash: try/teshhash.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ -pthread -g

clean:
	rm -rf *.o $(TARGETS)

pack:
	mkdir pack
	cp -r *.cpp *.h Makefile cityhash smhasher pack
	rm -rf cityhash/.git smhasher/.git
	zip -r9 pack pack
	rm -rf pack
