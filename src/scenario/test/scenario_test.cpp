/* 
 * File:   monitoringtest.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on May 4, 2010, 4:10 PM
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE monitoring_module_test
#define BOOST_AUTO_TEST_MAIN

#include <boost/scoped_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <list>
#include <string>
#include <boost/date_time/posix_time/time_parsers.hpp>


#include <exceptions.hpp>
#include <scenario/scenario.hpp>
#include <scenario/implementation.hpp>


using namespace std;

struct scenario_fixture_test {

    scenario_fixture_test() {
    }

    ~scenario_fixture_test() {
    }
};


/* db_connection testing */
BOOST_GLOBAL_FIXTURE(scenario_fixture_test);

BOOST_AUTO_TEST_SUITE(target_test)
BOOST_AUTO_TEST_CASE(target_default_test) {
    target t;
    BOOST_CHECK(!t.isAll());
    BOOST_CHECK_EQUAL(t.get_providers().size(), 0);
}

BOOST_AUTO_TEST_CASE(target_usage_test) {
    target t;
    string p1 = "192.168.0.1";
    string p2 = "192.168.0.2";
    string p3 = "192.168.0.3";

    BOOST_CHECK(!t.isAll());
    t.enableAll();
    BOOST_CHECK(t.isAll());

    t.add_provider(p1);
    t.add_provider(p2);
    t.add_provider(p3);
    BOOST_CHECK(t.isAll());
    BOOST_CHECK_EQUAL(t.get_providers().size(), 3);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(time_interval_test)
BOOST_AUTO_TEST_CASE(time_interval_default_test) {
    time_interval interval;

    BOOST_CHECK_THROW(interval.get_begin(), dd_exception);
    BOOST_CHECK_THROW(interval.get_end(), dd_exception);
}

BOOST_AUTO_TEST_CASE(time_interval_usage_test) {
    time_interval interval;
    const string startString = "2002-01-20 23:59:59.000";
    const string endString = "2002-01-21 23:59:59.000";

    boost::posix_time::ptime start = boost::posix_time::time_from_string(
            startString);
    boost::posix_time::ptime end = boost::posix_time::time_from_string(
            endString);

    BOOST_CHECK_THROW(interval.set_begin(interval.CURRENT_TIME + 1),
            dd_exception);
    BOOST_CHECK_THROW(interval.set_end(interval.CURRENT_TIME + 1),
            dd_exception);

    BOOST_CHECK_NO_THROW(interval.set_begin(interval.CURRENT_TIME));
    BOOST_CHECK_NO_THROW(interval.set_end(interval.CURRENT_TIME));

    interval.set_begin(start);
    interval.set_end(end);

    BOOST_CHECK(interval.get_begin() - start ==
            boost::posix_time::time_duration(0, 0, 0, 0));
    BOOST_CHECK(interval.get_end() - end ==
            boost::posix_time::time_duration(0, 0, 0, 0));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(condition_test)
BOOST_AUTO_TEST_CASE(dummy_default_test) {
    condition_general c;

    BOOST_CHECK_CLOSE(c.get_weight(), 0., 10e-10);
    BOOST_CHECK_CLOSE(c.get_lower_limit(), 0., 10e-10);
    BOOST_CHECK_CLOSE(c.get_upper_limit(), 0., 10e-10);
}

BOOST_AUTO_TEST_CASE(dummy_usage_test) {
    condition_general c;
    const weight_t w = 0.7;
    const weight_t wrong_w = 1.3;
    const double lower = 1.1;
    const double upper = 1.3;

    BOOST_CHECK_THROW(c.set_weight(wrong_w), dd_exception);
    BOOST_CHECK_NO_THROW(c.set_weight(w))
    BOOST_CHECK_CLOSE(c.get_weight(), w, 10e-10);

    c.set_lower_limit(lower);
    BOOST_CHECK_CLOSE(c.get_lower_limit(), lower, 10e-10);
    c.set_upper_limit(upper);
    BOOST_CHECK_CLOSE(c.get_upper_limit(), upper, 10e-10);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(factor_test)
BOOST_AUTO_TEST_CASE(dummy_default_test) {
    factor_general f;
    target t;

    BOOST_CHECK_CLOSE(f.get_weight(), 0., 10e-10);
    BOOST_CHECK_EQUAL(f.get_conditions_count(), 0);
    BOOST_CHECK_THROW(f.get_value(t), dd_exception);
}

BOOST_AUTO_TEST_CASE(dummy_usage_test) {
    factor_general f;
    condition_general *c1, *c2;
    boost::scoped_ptr<monitoring_data_result> result;
    libconfig::Config config;
    target t;

    const double lower = 0;
    const double upper = 200000;
    const weight_t w = 0.9;
    const weight_t wrong_w = 1.3;
    const string config_path = "../test.cfg";

    BOOST_CHECK_THROW(f.set_weight(wrong_w), dd_exception);
    BOOST_CHECK_NO_THROW(f.set_weight(w));
    BOOST_CHECK_CLOSE(f.get_weight(), w, 10e-10);

    c1 = new condition_general();
    c1->set_lower_limit(lower);
    c1->set_upper_limit(upper);
    c2 = new condition_general();
    c2->set_lower_limit(lower);
    c2->set_upper_limit(upper);

    // c1 & c2 becomes property of f
    config.readFile(config_path.c_str());
    f.set_config(&config);
    f.add_condition(c1);
    f.add_condition(c2);

    result.reset(f.get_value(t));
    for (list_record_t::const_iterator it = result->get_begin();
            it != result->get_end(); ++it) {
        record_t *record = *it;
        BOOST_MESSAGE("Timestamp: " +
                boost::posix_time::to_simple_string(record->first) +
                "\tValue: " + boost::lexical_cast<string > (record->second));

    }
    BOOST_CHECK_EQUAL(result->get_count(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(scenario_test)
BOOST_AUTO_TEST_CASE(default_test) {
    libconfig::Config config;
    const string config_path = "../monitoring/test/test.cfg";

    config.readFile(config_path.c_str());
    scenario s(&config);

    BOOST_MESSAGE("*** The following error message is OK.");
    BOOST_CHECK_EQUAL(s.get_score(), 0);
}

BOOST_AUTO_TEST_CASE(usage_test) {
    factor_general *f;
    condition_general *c1, *c2;
    libconfig::Config config;

    const double lower = 0;
    const double upper = 200000;
    const weight_t factor_weight = 1;
    const weight_t cond1_weight = 0.5;
    const weight_t cond2_weight = 0.8;
    const string config_path = "../monitoring/test/test.cfg";

    config.readFile(config_path.c_str());
    scenario s(&config);

    c1 = new condition_general();
    c1->set_lower_limit(lower + 100000);
    c1->set_upper_limit(upper);
    c1->set_weight(cond1_weight);
    c2 = new condition_general();
    c2->set_lower_limit(lower);
    c2->set_upper_limit(upper);
    c2->set_weight(cond2_weight);

    // c1 & c2 becomes property of f
    f = new factor_general();
    f->set_weight(factor_weight);
    f->add_condition(c1);
    f->add_condition(c2);

    s.add_factor(f);
    BOOST_CHECK_CLOSE(s.get_score(), factor_weight * cond2_weight, 10e-3);
}

BOOST_AUTO_TEST_SUITE_END()

