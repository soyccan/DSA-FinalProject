CXX = g++
CXXFLAGS += -std=c++17 -MMD

TARGETS = run
OBJS = src/mail_class.o src/search_mail_functions.o src/run.o src/query.o
deps = $(OBJS:%.o=%.d)

release: CXXFLAGS += -DNDEBUG -O3
debugopt: CXXFLAGS += -g -O3
debug: CXXFLAGS += -g

release: $(TARGETS)
debugopt: $(TARGETS)
debug: $(TARGETS)


run: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	$(RM) src/*.o src/*.d $(TARGETS)

.PHONY: pack
pack:
	mkdir pack
	cp *.cpp *.h Makefile pack
	zip -r pack pack
	rm -rf pack

-include $(deps)
