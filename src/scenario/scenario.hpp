/* 
 * File:   scenario.hpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on June 9, 2010, 10:22 AM
 */

#ifndef _SCENARIO_HPP
#define	_SCENARIO_HPP

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <libconfig.h++>
#include <list>
#include <string>

using namespace std;

// Forward declarations
class monitoring_data_result;

// Typedefs - maybe classes in the future
typedef double weight_t;
typedef double score_t;

class time_interval {
public:
    static const int CURRENT_TIME = 1;

    time_interval();
    ~time_interval();

    void set_begin(int curr);
    void set_begin(boost::posix_time::ptime begin);
    boost::posix_time::ptime get_begin();

    void set_end(int curr);
    void set_end(boost::posix_time::ptime end);
    boost::posix_time::ptime get_end();

    string to_string();
private:
    boost::posix_time::ptime begin_, end_;
};

class target {
public:
    target();
    virtual ~target();

    void add_provider(string &provider);

    list<string> get_providers() {
        return providers_;
    }

    void reset_providers() {
        providers_.clear();
    }

    void enableAll() {
        all_ = true;
    }

    void enableEvery() {
        every_ = true;
    }

    bool isAll() {
        return all_;
    }

    bool isEvery() {
        return every_;
    }

    string to_string();

private:
    bool all_;
    bool every_;
    list<string> providers_;
};

/**
 * The condition class is an abstract class for specifying conditions.
 * The subclasses should implement the is_true() method which must return
 * true if the condition is met based on the monitoring data provided
 * or false otherwise.
 */
class condition {
public:
    condition();
    virtual ~condition();

    weight_t get_weight() {
        return weight_;
    }
    void set_weight(weight_t weight);

    string get_name() {
        return name_;
    }

    void set_name(string &name) {
        name_ = name;
    }

    double get_upper_limit() {
        return upper_;
    }

    void set_upper_limit(double upper) {
        upper_ = upper;
    }

    double get_lower_limit() {
        return lower_;
    }

    void set_lower_limit(double lower) {
        lower_ = lower;
    }

    string to_string();

    virtual bool is_true(monitoring_data_result *value,
            time_interval interval) = 0;

protected:
    // Weight condition of factor
    weight_t weight_;
    string name_;

    // Limits of this condition so it can be true
    double lower_, upper_;
};

/**
 * The factor class is an abstract class for specifying factors. The subclasses
 * should implement the get_value() method which must return the monitoring
 * data results from the monitoring module. Based on the return values of this
 * function the conditions will be evaluated.
 */
class factor {
public:
    factor();
    virtual ~factor();

    void set_config(libconfig::Config *config);
    void set_repository(int repo);

    weight_t get_weight() {
        return weight_;
    }
    void set_weight(weight_t w);

    /**
     * Add a condition to this factor.
     * The caller is responsible in keeping the
     * condition object alive as much as necessary.
     */
    void add_condition(condition *cond) {
        conditions_.push_back(cond);
    }

    /**
     * The condition is the property of the callee.
     * NB: do *not* attempt to free the condition object retrieved using
     * these two methods
     */
    list<condition *>::const_iterator get_conditions_begin() {
        return conditions_.begin();
    }

    list<condition *>::const_iterator get_conditions_end() {
        return conditions_.end();
    }

    unsigned int get_conditions_count() {
        return conditions_.size();
    }

    void set_name(string name) {
        name_ = name;
    }

    string get_name() {
        return name_;
    }

    void set_interval(time_interval &interval) {
        interval_ = interval;
    }
    time_interval get_interval() {
        return interval_;
    }

    string to_string();


    /**
     * Beware that this method is *NOT* lazy and does *NOT* cache the result,
     * so keep the result if you do not actually need an updated value.
     */
    virtual monitoring_data_result* get_value(target t) = 0;

protected:
    // Weigth factor out of total
    weight_t weight_;
    string name_;
    int repo_type_;
    list<condition *> conditions_;
    time_interval interval_;
    libconfig::Config *config_;
};

/**
 * The scenario class is where the magic happens. All the classes above
 * are glued together getting a score for us. Just call get_score() and Yay!
 *
 */
class scenario {
public:
    scenario(libconfig::Config *config);
    virtual ~scenario();

    /**
     * Add a factor. The caller is responsible in keeping the factor object
     * alive as much as necessary.
     */
    void add_factor(factor *factor);

    void set_target(target &t) {
        target_ = t;
    }

    target get_target() {
        return target_;
    }

    string to_string();

    /**
     * Beware that this method is *NOT* lazy, so keep the result if you do
     * not actually need an updated score. Each time this method is called,
     * the monitoring data repository gets analyzed.
     * (i.e. pretty expensive operation)
     *
     * Can be implemented differently if you need to by extending the class.
     * In this basic class,the algorithm used to compute the score is
     * described at http://loopback.ro/wiki/blobseer/doku.php#scoring_algorithm
     */
    virtual score_t get_score();

private:
    list<factor *> factors_;
    weight_t total_weight_;
    target target_;
    libconfig::Config *config_;
};



#endif	/* _SCENARIO_HPP */

