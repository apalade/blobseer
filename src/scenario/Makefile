OBJS=scenario.o implementation.o scenario_test.o
ALL_HEADERS=/pala/deploy/include
ALL_LIBS=/pala/deploy/lib
BOOST_LIB=-lboost_unit_test_framework -lboost_date_time
LIBCONFIG_LIB=-lconfig++
BLOBSEER_PATH=/pala/proiecte/blobseer/blobseer/trunk/
LOG_LEVEL=__INFO
DEBUG=-ggdb
SOCI_LIB=-lsoci_core -lsoci_postgresql
export SOCI_BACKENDS_PATH:=/usr/local/lib/

scenario: ${OBJS}
	g++ -Wall ${DEBUG} ${SOCI_LIB} ${BOOST_LIB} ${LIBCONFIG_LIB} \
		-L${ALL_LIBS} \
		../exceptions.o \
		../monitoring/monitoring_data.o ../monitoring/db_connection.o\
		scenario.o implementation.o scenario_test.o \
		-o test/scenario_test

scenario.o: scenario.cpp
	g++ -Wall ${DEBUG} -c scenario.cpp \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS} \
		-D${LOG_LEVEL}
implementation.o: implementation.cpp
	g++ -Wall ${DEBUG} -c implementation.cpp \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS}\
		-D${LOG_LEVEL}

scenario_test.o: test/scenario_test.cpp
	g++ -Wall ${DEBUG} -c test/scenario_test.cpp \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS}\
		-D${LOG_LEVEL}

test: scenario ${OBJS}
	./test/scenario_test --detect_memory_leaks --show-progress \
		--log_level=all

leaks: scenario ${OBJS}
	valgrind --read-var-info=yes --show-reachable=yes --leak-check=full \
				 --track-origins=yes ./test/scenario_test

clean:
	rm -rf *.o test/scenario_test
