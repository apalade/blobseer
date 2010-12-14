/* 
 * File:   monitoring_data.hpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on May 11, 2010, 10:04 AM
 */

#ifndef _MONITORING_DATA_HPP
#define	_MONITORING_DATA_HPP

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/unordered_set.hpp>
#include <libconfig.h++>
#include <list>
#include <string>

using namespace std;

// Forward declarations
class monitoring_data_result;
class db_connection;

typedef pair<boost::posix_time::ptime, double> record_t;
typedef list<record_t *> list_record_t;

/**
 * Interface for specifying the get_parameter method for the repositories
 */
class monitoring_data {
public:
    monitoring_data(libconfig::Config *config);

    monitoring_data_result* get_parameter(const string &param);
    void get_critical_providers(
            boost::unordered_set<string> *critical_providers);
    void set_targets(list<string> &targets);
    void set_interval(boost::posix_time::ptime &start,
            boost::posix_time::ptime &end);
    void set_aggregate(int agg_type);

    virtual ~monitoring_data();
protected:
    /**
     * Initialize database & other (lazy)
     * @see init_
     */
    bool initialize_();

    /**
     * Builds a template for SELECTs
     */
    string build_query_template_();

    /**
     * In the future, might need to specialize this function for each
     * of it's subclasses
     */
    virtual bool get_parameter_specific_(const string &param,
            monitoring_data_result *result);

    /**
     * Specific for each of the implementations
     *
     * @param returns the list of table names from which we should get data from
     */
    virtual void build_table_names_(list<string> *tables);


    libconfig::Config *config_;
    // Lazy initialization
    bool init_;
    // Connection to the database
    db_connection *conn_;

    list<string> targets_;
    time_t start_, end_;
    int agg_type_;
    int op_type_;
};

/**
 * Wrapper class for the results brought from the two repositories. Some
 * fields make sens only for one type of repository 
 *
 * About ownership of this class
 *  - this class is usually alloc'd by get_parameter()
 *  - the caller of this method has the ownership over the object created
 *  - do *NOT* free the record list/pair manually, just make sure you free
 * the instance once your done, if it was alloc'd manually
 */
class monitoring_data_result {
public:
    /**
     * Add a record for this result instance
     */
    void add_record(boost::posix_time::ptime rectime, double value);

    /**
     * Simple iterator accessors
     */
    list_record_t::const_iterator get_begin() {
        return this->records_->begin();
    }

    list_record_t::const_iterator get_end() {
        return this->records_->end();
    }

    unsigned int get_count() {
        return this->records_->size();
    }

    monitoring_data_result();
    virtual ~monitoring_data_result();
private:
    // List of entries  (record time, record value)
    list_record_t *records_;

};

/**
 * Specific implementation for the Internal repository
 */
class internal_monitoring_data : public monitoring_data {
public:
    internal_monitoring_data(libconfig::Config *config);
    void set_operation_type(int type);
    virtual ~internal_monitoring_data();
private:
    void build_table_names_(list<string> *tables);

};

/**
 * Specific implementation for the Physical repository
 */
class physical_monitoring_data : public monitoring_data {
public:
    physical_monitoring_data(libconfig::Config *config);
    virtual ~physical_monitoring_data();
private:
    void build_table_names_(list<string> *tables);

};

#endif	/* _MONITORING_DATA_HPP */

