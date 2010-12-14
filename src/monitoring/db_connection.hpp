/* 
 * File:   DbConnection.h
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on May 4, 2010, 5:15 PM
 */

#ifndef _DBCONNECTION_H
#define	_DBCONNECTION_H

#include <boost/scoped_ptr.hpp>
#include <soci.h>
#include <string>

using namespace std;

class db_connection {
public:
    db_connection(std::string &dsn);

    /**
     * Helper function to create a dsn
     */
    static std::string build_dsn(const string &driver, const string &name,
        const string &host,const string &port,
        const string &user, const string &pass);
    
    /**
     * Run the query and return an iterator over the rowset associated
     * with the query
     *
     * @return if the return value is NULL then the query was
     * not executed (a database connection-related problem appeared)
     */
    soci::rowset<soci::row>* get_rows(std::string &query);

    /**
     * How many times are we allowed to reuse a specific database session
     * defaults to 100
     */
    void set_session_max_reuse(size_t max_used) {
        max_used_ = max_used;
    }

    /**
     * How many times the object is allowed to retry the same query before
     * giving up
     *
     * defaults to 5
     */
    void set_query_max_retry(size_t max_retries) {
        max_retries_ = max_retries;
    }

    /**
     * How many seconds to wait before retrying the same query in case of an 
     * error.
     *
     * NB: The value you are giving to use will be multiplied by how many times
     * we are retrying (e.g.: for the first retry with the wait parameter of 
     * 1.5s it will wait for 1.5s, but for the 3rd retry with the same wait
     * parameter, it will wait for 4.5s)
     *
     * defaults to 1.0s
     */
    void set_wait_factor(float wait) {
        this->wait_before_retrying_ = (long) (wait * 1000000);
    }

    virtual ~db_connection();
private:
    // Database session
    boost::scoped_ptr<soci::session> session_;
    // How many times did we use the current session
    size_t count_;
    // How many times we are allowed to use this session
    size_t max_used_;
    // How many times we are allowed to try one query before giving up
    size_t max_retries_;
    // How many microseconds should we wait before retrying the query
    long wait_before_retrying_;

    /**
     * Reconnect the session.
     *
     * @return false if we failed to do this
     */
    bool reconnect_(void);

    /**
     * Refresh the session if needed
     *
     * @return false if we failed to do this
     */
    bool refresh_(void);
};

#endif	/* _DBCONNECTION_H */

