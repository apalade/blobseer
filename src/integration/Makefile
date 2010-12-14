OBJS=mover.o poolmanager.o integration_test.o comm.o
ALL_HEADERS=/pala/deploy/include
ALL_LIBS=/pala/deploy/lib
BOOST_LIB=-lboost_unit_test_framework \
	  -lboost_date_time -lboost_thread \
	  -lboost_system -lboost_serialization
LIBCONFIG_LIB=-lconfig++
BLOBSEER_PATH=/pala/proiecte/blobseer/blobseer/trunk/
LOG_LEVEL=__INFO
SOCK_TYPE=tcp
DEBUG=-ggdb
SOCI_LIB=-lsoci_core -lsoci_postgresql
DEFINES=-D__DEBUG -D${LOG_LEVEL} -DSOCK_TYPE=${SOCK_TYPE}
export SOCI_BACKENDS_PATH:=/usr/local/lib/


integration: ${OBJS}
	g++ -Wall ${DEBUG} ${BOOST_LIB} ${LIBCONFIG_LIB} ${SOCI_LIB} \
		-L${ALL_LIBS} \
		../exceptions.o \
		../scenario/scenario.o ../scenario/implementation.o \
		../monitoring/monitoring_data.o ../monitoring/db_connection.o \
		${OBJS} \
		-o test/integration_test

mover.o: mover.cpp
	g++ -Wall ${DEBUG} -c mover.cpp \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS} \
		${DEFINES}

poolmanager.o: poolmanager.cpp
	g++ -Wall ${DEBUG} -c poolmanager.cpp \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS}\
		${DEFINES}

comm.o: comm.cpp
	g++ -Wall ${DEBUG} -c comm.cpp \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS}\
		${DEFINES}
		

integration_test.o: test/integration_test.cpp
	g++ -Wall ${DEBUG} -c test/integration_test.cpp \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS}\
		${DEFINES}

test: integration ${OBJS}
	./test/integration_test --detect_memory_leaks --show-progress \
		--log_level=all

leaks: integration ${OBJS}
	valgrind --read-var-info=yes --show-reachable=yes --leak-check=full \
				 --track-origins=yes ./test/integration_test

clean:
	rm -rf *.o test/integration_test
