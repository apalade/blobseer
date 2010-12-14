/* 
 * File:   implementation.hpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on June 10, 2010, 2:58 PM
 */

#ifndef _IMPLEMENTATION_HPP
#define	_IMPLEMENTATION_HPP

#include "monitoring/monitoring_data.hpp"
#include "scenario.hpp"

class condition_general: public condition {
public:
    bool is_true(monitoring_data_result *values, time_interval interval);
};

class factor_general: public factor {
public:
    monitoring_data_result* get_value(target t);
};


class condition_rws: public condition {
public:
    bool is_true(monitoring_data_result *values, time_interval interval);
};

class factor_read: public factor {
public:
    monitoring_data_result* get_value(target t);
};

class factor_write: public factor {
public:
    monitoring_data_result* get_value(target t);
};

#endif	/* _IMPLEMENTATION_HPP */

