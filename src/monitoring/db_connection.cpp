/* 
 * File:   db_connection.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 * 
 * Created on May 4, 2010, 5:15 PM
 */

#include <string>

#include "common/debug.hpp"

#include "db_connection.hpp"


db_connection::db_connection(std::string &dsn) : count_(0),
        max_used_(100), max_retries_(5), wait_before_retrying_(1000000) {
    try {
        this->session_.reset(new soci::session(dsn));
    } catch (soci::soci_error const &e) {
        ERROR("Failed to connect to the database (dsn: " << dsn << "). "
                "Error was: " << e.what());
    }
}

std::string db_connection::build_dsn(const string &driver,
        const string &name,
        const string &host, const string &port,
        const string &user, const string &pass) {
    std::string dsn;

    dsn = driver + "://";
    if (!host.empty()) {
        dsn += "host=" + host + " ";
    }
    if (!port.empty()) {
        dsn += "port=" + port + " ";
    }
    if (!user.empty()) {
        dsn += "user=" + user + " ";
    }
    if (!pass.empty()) {
        dsn += "password=" + pass + " ";
    }
    if (!name.empty()) {
        dsn += "dbname=" + name + " ";
    }
    
    return dsn;
}

soci::rowset<soci::row>* db_connection::get_rows(std::string& query) {
    // How many times did we try reconnecting for this query
    size_t nr_retries;

    // Check session if it is valid
    if (NULL == this->session_.get()) {
        return NULL;
    }

    // Refresh the session if need it
    if (!this->refresh_()) {
        // Can't reuse it - an error appeared
        return NULL;
    }

    nr_retries = 0;
    while (nr_retries++ < max_retries_) {
        try {
            return new soci::rowset<soci::row >
                    (this->session_->prepare << query);
        } catch (soci::soci_error const &e) {
            INFO("Could not run query [" << query << "], trying to "
                    "reconnect to the database. Error was: " << e.what());
            if (!this->reconnect_()) {
                usleep(this->wait_before_retrying_ * nr_retries);
            }
        }
    }

    ERROR("Tried reconnecting to the database " << max_retries_ << " times "
            "before giving up.");
    return NULL;
}

/*
void db_connection::execute(std::string& query) {
    // How many times did we try reconnecting for this query
    size_t nr_retries;

    // Check session if it is valid
    if (NULL == this->session_.get()) {
        return NULL;
    }

    // Refresh the session if need it
    if (!this->refresh_()) {
        // Can't reuse it - an error appeared
        return NULL;
    }

    nr_retries = 0;
    while (nr_retries++ < max_retries_) {
        try {
            this->session_->prepare << query;
        } catch (soci::soci_error const &e) {
            INFO("Could not run a query, trying to reconnect to the database. "
                    "Error was: " << e.what());
            if (!this->reconnect_()) {
                usleep(this->wait_before_retrying_ * nr_retries);
            }
        }
    }

    ERROR("Tried reconnecting to the database " << max_retries_ << " times "
            "before giving up.");
    return NULL;
}
*/


bool db_connection::reconnect_(void) {
    try {
        this->session_->reconnect();
        return true;
    } catch (soci::soci_error const &e) {
        ERROR("Failed to reconnect to the database. "
                "Error was: " << e.what());
    }

    return false;
}

bool db_connection::refresh_(void) {
    size_t nr_retries;
    long wait;

    if (this->count_++ < this->max_used_) {
        // This session can still be used
        return true;
    }

    INFO("This session expired after " << this->max_used_ << " uses."
            " Reconnecting to the database");
    nr_retries = 0;
    while (nr_retries++ < this->max_retries_) {
        if (this->reconnect_()) {
            return true;
        }
        wait = this->wait_before_retrying_ * nr_retries;
        INFO("Failed to reconnect to the database, "
                "sleeping for " << (float) (wait / 1000000) << " seconds "
                "before retrying");
        usleep(wait);
    }

    ERROR("Failed to renew the connection "
            "after " << this->max_retries_ << " attempts. Giving up.");

    return false;
}

db_connection::~db_connection() {
    try {
        this->session_->close();
    } catch (soci::soci_error const &e) {
        ERROR("Failed to close the database connection. "
                "Error was: " << e.what());
    }
}

