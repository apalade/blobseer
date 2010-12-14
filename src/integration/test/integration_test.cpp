/* 
 * File:   monitoringtest.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on May 4, 2010, 4:10 PM
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE integration_module_test
#define BOOST_AUTO_TEST_MAIN

#include <boost/date_time/posix_time/time_parsers.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <libconfig.h++>
#include <list>
#include <string>

#include "integration/comm.hpp"
#include "integration/mover.hpp"
#include "integration/poolmanager.hpp"

using namespace std;

struct integration_fixture_test {

    integration_fixture_test() {
    }

    ~integration_fixture_test() {
    }
};


/* db_connection testing */
BOOST_GLOBAL_FIXTURE(integration_fixture_test);

BOOST_AUTO_TEST_SUITE(poolmanager_test)
BOOST_AUTO_TEST_CASE(poolmanager_default_test) {
    libconfig::Config config;
    const string config_path = "test.cfg";

    try {
        config.readFile(config_path.c_str());
        poolmanager pm(&config);

        pm.run();
    } catch (libconfig::SettingTypeException ex) {
        ERROR(ex.getPath());
    } catch (libconfig::ParseException ex) {
        ERROR(ex.getError() << "(line: " << ex.getLine() << ")");
    }



}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(mover_test)
BOOST_AUTO_TEST_CASE(mover_default_test) {
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(comm_test)
BOOST_AUTO_TEST_CASE(comm_default_test) {
    libconfig::Config config;
    const string config_path = "test.cfg";
    list<string> providers;

    config.readFile(config_path.c_str());
    comm c(&config);
    providers = c.get_all_providers();

    for (list<string>::const_iterator it = providers.begin();
            it != providers.end(); ++it) {
        BOOST_MESSAGE("Received provider: " << *it);
    }
}
BOOST_AUTO_TEST_SUITE_END()

