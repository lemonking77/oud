all: target
FORCE: ;
.PHONY: FORCE

target:	oud

CFLAGS=
LDFLAGS=

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.cxx
	@$(CXX) $(CFLAGS) -c $< -o $@


oud: oud.o uart.o
	$(CC) $(LDFLAGS) $^ -o $@


clean:
	rm -f *.o $(target)
