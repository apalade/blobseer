OBJS=monitoring_data.o db_connection.o monitoring_test.o
ALL_HEADERS=/pala/deploy/include
ALL_LIBS=/pala/deploy/lib
SOCI_HEADER=/usr/local/include/soci
SOCI_LIB=-lsoci_core -lsoci_postgresql
POSTGRESQL_HEADER=/usr/include/postgresql
BOOST_LIB=-lboost_unit_test_framework -lboost_date_time
LIBCONFIG_LIB=-lconfig++
BLOBSEER_PATH=/pala/proiecte/blobseer/blobseer/trunk/
LOG_LEVEL=__INFO
DEBUG=-ggdb
export SOCI_BACKENDS_PATH:=/usr/local/lib/

monitoring: ${OBJS}
	g++ -Wall ${DEBUG} ${SOCI_LIB} ${BOOST_LIB} ${LIBCONFIG_LIB} \
		-L${ALL_LIBS} \
		monitoring_data.o db_connection.o \
		monitoring_test.o ../exceptions.o \
		-o test/monitoring_test


monitoring_data.o: monitoring_data.cpp
	g++ -Wall ${DEBUG} -c monitoring_data.cpp -I${SOCI_HEADER} \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS} \
		-I${POSTGRESQL_HEADER} -D${LOG_LEVEL}

db_connection.o: db_connection.cpp
	g++ -Wall ${DEBUG} -c db_connection.cpp -I${SOCI_HEADER} \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS} \
		-I${POSTGRESQL_HEADER} -D${LOG_LEVEL}

monitoring_test.o: test/monitoring_test.cpp
	g++ -Wall ${DEBUG} -c test/monitoring_test.cpp -I${SOCI_HEADER} \
		-I${BLOBSEER_PATH} -I../ -I${ALL_HEADERS} \
		-I${POSTGRESQL_HEADER} -D${LOG_LEVEL}

test: monitoring ${OBJS}
	./test/monitoring_test --detect_memory_leaks --show-progress \
		--log_level=all

leaks: monitoring ${OBJS}
	valgrind --read-var-info=yes --show-reachable=yes --leak-check=full --track-origins=yes ./test/monitoring_test

clean:
	rm -rf *.o test/monitoring_test
