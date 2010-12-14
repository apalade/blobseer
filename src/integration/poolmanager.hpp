/* 
 * File:   poolmanager.hpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on June 10, 2010, 3:54 PM
 */

#ifndef _POOLMANAGER_HPP
#define	_POOLMANAGER_HPP

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "integration/comm.hpp"
#include <libconfig.h++>
#include <list>
#include "monitoring/monitoring_data.hpp"
#include "scenario/scenario.hpp"
#include <string>

// Forward declaration
class mover;

/**
 * The poolmanager class is responsible for taking the decision to
 * enable/disable data providers
 */
class poolmanager {
public:
    poolmanager(libconfig::Config *config);
    virtual ~poolmanager();

    void run();
private:
    typedef boost::unordered_map<std::string, scenario *> scenarios_map_t;
    typedef boost::unordered_map<std::string, factor *> factors_map_t;
    typedef boost::unordered_map<std::string, condition *> conditions_map_t;
    typedef boost::unordered_map<std::string, target *> targets_map_t;
    typedef boost::unordered_map<std::string, time_interval *> intervals_map_t;


    void action_(scenario *s);
    list<string> get_active_providers_();
    void get_critical_providers_(
            boost::unordered_set<string> *critical_providers);
    void init_thresholds_();
    void init_misc_();
    void init_specifications_();
    void init_conditions_();
    void init_targets_();
    void init_time_intervals_();
    void init_factors_();
    void init_scenarios_();
    boost::posix_time::ptime normalize_time_(int seconds);


    libconfig::Config *config_;
    double add_threshold_, remove_threshold_;
    int sleep_no_active_, sleep_between_, max_cycles_;
    comm comm_;
    monitoring_data *monitoring_data_;
    mover *mover_;

    targets_map_t targets_by_name_;
    intervals_map_t intervals_by_name_;
    conditions_map_t conds_by_name_;
    factors_map_t factors_by_name_;
    scenarios_map_t scenarios_by_name_;
};

#endif	/* _POOLMANAGER_HPP */

