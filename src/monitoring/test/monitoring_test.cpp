/* 
 * File:   monitoringtest.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on May 4, 2010, 4:10 PM
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE monitoring_module_test
#define BOOST_AUTO_TEST_MAIN

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <list>
#include <soci.h>
#include <string>

#include "constants.hpp"
#include "monitoring/db_connection.hpp"
#include "monitoring/monitoring_data.hpp"


using namespace std;

string dsn = "postgresql://dbname=test user=root password=12345";
const string table = "test_table";
const string src_path = "/pala/proiecte/blobseer/mine/monitoring/";

struct db_fixture_test {
    soci::session *sql;

    db_fixture_test() {
        sql = new soci::session(dsn);
        int i;

        BOOST_TEST_MESSAGE("Settting up fixture in progress");
        *sql << "CREATE TABLE " << table << " (test_value INT)";
        soci::statement st = (sql->prepare << "INSERT INTO " << table << " "
                "VALUES (:val)", soci::use(i));
        for (i = 0; i < 10; ++i) {
            st.execute(true);
        }
    }

    ~db_fixture_test() {
        BOOST_TEST_MESSAGE("Destroying fixture in progress");
        *sql << "DROP TABLE " << table;
        delete sql;
    }
};


/* db_connection testing */
BOOST_GLOBAL_FIXTURE(db_fixture_test);

//BOOST_FIXTURE_TEST_SUITE(db_connection_test, db_fixture_test)

BOOST_AUTO_TEST_SUITE(db_connection_test)
BOOST_AUTO_TEST_CASE(basic_select_test) {
    int i;
    soci::rowset<soci::row>::const_iterator it;
    string query;

    query = "SELECT * FROM " + table;
    boost::scoped_ptr<db_connection> conn(new db_connection(dsn));
    boost::scoped_ptr<soci::rowset<soci::row> > rows;

    rows.reset(conn->get_rows(query));
    for (i = 0, it = rows->begin(); it != rows->end(); ++i, ++it) {
        soci::row const& row = *it;
        BOOST_CHECK_EQUAL(row.get<int>(0), i);
    }
}

/*
BOOST_AUTO_TEST_CASE(wrong_host_test) {
    string dsn_bad = "postgresql://host=WRONGHOST dbname=test "
            "user=pala password=12345";
    string query = "SELECT * FROM " + table;

    db_connection *conn = new db_connection(dsn_bad);
    BOOST_CHECK(conn->get_rows(query) == NULL);
}
 */
BOOST_AUTO_TEST_CASE(build_dsn_test) {
    const string dsn1 = "driver://host=host port=port user=user password=pass dbname=name ";
    const string dsn2 = "driver://host=host port=port user=user dbname=name ";
    const string dsn3 = "driver://host=host port=port password=pass dbname=name ";
    const string dsn4 = "driver://host=host user=user password=pass dbname=name ";
    const string dsn5 = "driver://port=port user=user password=pass dbname=name ";
    const string dsn6 = "driver://dbname=name ";

    const string dsn1_built = db_connection::
            build_dsn("driver", "name", "host", "port", "user", "pass");
    const string dsn2_built = db_connection::
            build_dsn("driver", "name", "host", "port", "user", "");
    const string dsn3_built = db_connection::
            build_dsn("driver", "name", "host", "port", "", "pass");
    const string dsn4_built = db_connection::
            build_dsn("driver", "name", "host", "", "user", "pass");
    const string dsn5_built = db_connection::
            build_dsn("driver", "name", "", "port", "user", "pass");
    const string dsn6_built = db_connection::
            build_dsn("driver", "name", "", "", "", "");


    BOOST_REQUIRE_EQUAL(dsn1, dsn1_built);
    BOOST_REQUIRE_EQUAL(dsn2, dsn2_built);
    BOOST_REQUIRE_EQUAL(dsn3, dsn3_built);
    BOOST_REQUIRE_EQUAL(dsn4, dsn4_built);
    BOOST_REQUIRE_EQUAL(dsn5, dsn5_built);
    BOOST_REQUIRE_EQUAL(dsn6, dsn6_built);
}

BOOST_AUTO_TEST_SUITE_END()


// Monitoring data from MonALISA
BOOST_AUTO_TEST_SUITE(monitoring_data_monalisa_test)
BOOST_AUTO_TEST_CASE(get_params) {
    const string configFile = src_path + "../test.cfg";
    const string startString = "2002-01-20 23:59:59.000";
    const string endString = "2010-10-21 23:59:59.000";
    const string paramInternal = "blob_id";
    const string paramPhysical = "load5";
    int count;
    monitoring_data_result *result;

    libconfig::Config cfg;
    cfg.readFile(configFile.c_str());

    list<string> targets;
    targets.push_back("127.0.0.1");

    boost::posix_time::ptime start(
            boost::posix_time::time_from_string(startString));
    boost::posix_time::ptime end(boost::posix_time::time_from_string(endString));

    boost::scoped_ptr<internal_monitoring_data> data_internal(
            new internal_monitoring_data(&cfg));
    data_internal->set_targets(targets);
    data_internal->set_operation_type(monitoring_const::INTERNAL_TYPE_READ);
    data_internal->set_aggregate(monitoring_const::AGGREGATE_NONE);
    data_internal->set_interval(start, end);
    result = data_internal->get_parameter(paramInternal);

    count = 0;
    for (list_record_t::const_iterator i = result->get_begin();
            i != result->get_end(); ++i) {
        pair<boost::posix_time::ptime, double> *record = *i;
        cout << record->first << "\t" << record->second << endl;
        ++count;
    }

    // At least one record
    BOOST_REQUIRE_GT(count, 0);
    BOOST_MESSAGE("We found a total of " << count << " entries");
    delete result;

    boost::scoped_ptr<physical_monitoring_data> data_physical(
            new physical_monitoring_data(&cfg));
    data_physical->set_targets(targets);
    data_physical->set_aggregate(monitoring_const::AGGREGATE_NONE);
    data_physical->set_interval(start, end);
    result = data_physical->get_parameter(paramPhysical);

    count = 0;
    for (list_record_t::const_iterator i = result->get_begin();
            i != result->get_end(); ++i) {
        pair<boost::posix_time::ptime, double> *record = *i;
        cout << record->first << "\t" << record->second << endl;
        ++count;
    }

    // At least one record
    BOOST_REQUIRE_GT(count, 0);
    BOOST_MESSAGE("We found a total of " << count << " entries");
    delete result;
}

BOOST_AUTO_TEST_SUITE_END()
