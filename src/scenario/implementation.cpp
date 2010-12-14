/* 
 * File:   implementation.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 * 
 * Created on June 10, 2010, 2:58 PM
 */
#include <boost/scoped_ptr.hpp>
#define __DEBUG
#include <common/debug.hpp>

#include <constants.hpp>
#include <exceptions.hpp>
#include <boost/date_time/time_duration.hpp>

#include "implementation.hpp"

bool condition_general::is_true(monitoring_data_result* values,
        time_interval interval) {
    // You can access values like this:
    for (list_record_t::const_iterator it = values->get_begin();
            it != values->get_end(); ++it) {
        record_t *record = *it;
        //record->first;            // Timestamp     boost::posix_time::ptime
        //record->second;           // Value         double

        // If you return true, it usually means that the value (whatever that is)
        //  is in (lower, upper) limits and the condition is said to be satisfied
        if (record->second >= this->get_lower_limit() &&
                record->second <= this->get_upper_limit()) {
            return true;
        }
    }

    return false;
}

monitoring_data_result* factor_general::get_value(target t) {
    if (NULL == config_) {
        throw dd_exception("No configuration file provided for factor.");
    }
    list<string> targets = t.get_providers();
    boost::posix_time::ptime start = interval_.get_begin();
    boost::posix_time::ptime end = interval_.get_end();


    switch (repo_type_) {
        case monitoring_const::REPO_INTERNAL:
        {
            boost::scoped_ptr<internal_monitoring_data> data(
                    new internal_monitoring_data(config_));
            data->set_operation_type(monitoring_const::INTERNAL_TYPE_READ);
            data->set_aggregate(monitoring_const::AGGREGATE_NONE);
            data->set_targets(targets);
            data->set_interval(start, end);
            return data->get_parameter(name_);
        }
        case monitoring_const::REPO_PHYSICAL:
        {
            boost::scoped_ptr<physical_monitoring_data> data(
                    new physical_monitoring_data(config_));
            data->set_aggregate(monitoring_const::AGGREGATE_NONE);
            data->set_targets(targets);
            data->set_interval(start, end);
            return data->get_parameter(name_);
        }
        default:
            ERROR("Repository type in factor in wrong.");
    }

    return NULL;
}


// ----------- Read/Write per time unit ---------------------

bool condition_rws::is_true(monitoring_data_result* values,
        time_interval interval) {
    if (interval.get_begin().is_not_a_date_time() ||
            interval.get_end().is_not_a_date_time()) {
        return false;
    }

    boost::posix_time::time_duration t = interval.get_end() -
            interval.get_begin();
    double cps = values->get_count() * 1. / t.total_seconds();
    DBG("CPS: " << cps << "(" << values->get_count() << ", "
            << t.total_seconds() << ", " 
            << this->get_lower_limit() << ", "
            << this->get_upper_limit() << " ) ");
    if (cps >= this->get_lower_limit() && cps <= this->get_upper_limit()) {
        return true;
    }
    return false;
}

monitoring_data_result* factor_read::get_value(target t) {
    if (NULL == config_) {
        throw dd_exception("No configuration file provided for factor.");
    }
    list<string> targets = t.get_providers();
    boost::scoped_ptr<internal_monitoring_data> data(
            new internal_monitoring_data(config_));
    boost::posix_time::ptime start = interval_.get_begin();
    boost::posix_time::ptime end = interval_.get_end();

    data->set_operation_type(monitoring_const::INTERNAL_TYPE_READ);
    data->set_aggregate(monitoring_const::AGGREGATE_NONE);
    data->set_targets(targets);
    data->set_interval(start, end);
    return data->get_parameter(name_);
}

monitoring_data_result* factor_write::get_value(target t) {
    if (NULL == config_) {
        throw dd_exception("No configuration file provided for factor.");
    }
    list<string> targets = t.get_providers();
    boost::scoped_ptr<internal_monitoring_data> data(
            new internal_monitoring_data(config_));
    boost::posix_time::ptime start = interval_.get_begin();
    boost::posix_time::ptime end = interval_.get_end();

    data->set_operation_type(monitoring_const::INTERNAL_TYPE_WRITE);
    data->set_aggregate(monitoring_const::AGGREGATE_NONE);
    data->set_targets(targets);
    data->set_interval(start, end);
    return data->get_parameter(name_);
}