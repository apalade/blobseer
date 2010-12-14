export SOCI_BACKENDS_PATH:=/usr/local/lib/

all: exceptions.o constants.hpp
	$(MAKE) -C monitoring
	$(MAKE) -C scenario
	$(MAKE) -C integration

clean:
	$(MAKE) clean -C monitoring
	$(MAKE) clean -C scenario
	$(MAKE) clean -C integration
	rm exceptions.o

test_integ: all
	./integration/test/integration_test --detect_memory_leaks --show-progress \
		--log_level=all
	

exceptions.o:
	gcc -Wall -c exceptions.cpp
