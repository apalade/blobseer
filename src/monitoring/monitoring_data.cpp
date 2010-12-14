/* 
 * File:   monitoring_data.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 * 
 * Created on May 11, 2010, 10:04 AM
 */

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <libconfig.h++>
#include <list>

#include "common/debug.hpp"
#include "constants.hpp"
#include "db_connection.hpp"

#include "monitoring_data.hpp"

using namespace std;

// -------------- Start monitoring_data -----------------

monitoring_data::monitoring_data(libconfig::Config *config) :
        config_(config), start_(0), end_(0),
        agg_type_(monitoring_const::AGGREGATE_NONE),
        op_type_(monitoring_const::INTERNAL_TYPE_READ) {
}

void monitoring_data::set_interval(boost::posix_time::ptime& start,
        boost::posix_time::ptime& end) {
    tm start_tm, end_tm;
    if (start.is_not_a_date_time() || end.is_not_a_date_time()) {
        return;
    }
    start_tm = boost::posix_time::to_tm(start);
    end_tm = boost::posix_time::to_tm(end);
    start_ = mktime(&start_tm);
    end_ = mktime(&end_tm);
}

void monitoring_data::set_targets(list<string> &targets) {
    this->targets_ = targets;
}

void monitoring_data::set_aggregate(int agg_type) {
    this->agg_type_ = agg_type;
}

monitoring_data_result* monitoring_data::get_parameter(const string &param) {
    monitoring_data_result *result;

    // Lazy initialization if it's the case
    if (!this->init_) {
        if (!this->initialize_()) {
            // Bummer!
            return NULL;
        }
    }

    result = new monitoring_data_result();
    get_parameter_specific_(param, result);

    return result;
}

void monitoring_data::get_critical_providers(
        boost::unordered_set<string> *critical_providers) {
    string query;

    // Lazy initialization if it's the case
    if (!this->init_) {
        if (!this->initialize_()) {
            // Bummer!
            return;
        }
    }

    query = "SELECT DISTINCT"
            "   client_id "
            "FROM "
            "   created_blobs "
            "WHERE"
            "   replication = 1";
    boost::scoped_ptr<soci::rowset<soci::row> > rows;
    rows.reset(this->conn_->get_rows(query));
    for (soci::rowset<soci::row>::const_iterator it = rows->begin();
            it != rows->end(); ++it) {
        critical_providers->insert(it->get<string > (0));
    }
}

bool monitoring_data::initialize_() {
    string db_host, db_port, db_user, db_pass, db_driver, db_name;
    string dsn;
    const string repo_path = MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SECTION_REPO);
    const string repo_db_path = MERGE_PATH(repo_path,
            config_const::SECTION_DB);

    if (!this->config_->lookupValue(MERGE_PATH(repo_db_path,
            config_const::SECTION_DB_DRIVER), db_driver) ||
            !this->config_->lookupValue(MERGE_PATH(repo_db_path,
            config_const::SECTION_DB_HOST), db_host) ||
            !this->config_->lookupValue(MERGE_PATH(repo_db_path,
            config_const::SECTION_DB_PORT), db_port) ||
            !this->config_->lookupValue(MERGE_PATH(repo_db_path,
            config_const::SECTION_DB_NAME), db_name) ||
            !this->config_->lookupValue(MERGE_PATH(repo_db_path,
            config_const::SECTION_DB_USER), db_user) ||
            !this->config_->lookupValue(MERGE_PATH(repo_db_path,
            config_const::SECTION_DB_PASS), db_pass)) {
        ERROR("Please check the database section for DD module "
                "repository to have all the options");
        return false;
    }

    // Make connection to the database
    dsn = db_connection::build_dsn(db_driver, db_name, db_host,
            db_port, db_user, db_pass);
    this->conn_ = new db_connection(dsn);

    return true;
}

string monitoring_data::build_query_template_() {
    string query;
    query = "SELECT ";
    switch (this->agg_type_) {
        case monitoring_const::AGGREGATE_AVG:
            query += "AVG(rectime), AVG(\"%s\") ";
            break;
        case monitoring_const::AGGREGATE_SUM:
            query += "AVG(rectime), SUM(\"%s\") ";
            break;
        case monitoring_const::AGGREGATE_NONE:
            query += "rectime, \"%s\" ";
            break;
        default:
            ERROR("Aggregation type is wrong. Value: " + this->agg_type_);
            return "";
    }
    query += ""
            "FROM "
            "   \"%s\" ";

    if (this->start_ != 0 && this->end_ != 0) {
        query += ""
                "WHERE "
                "   rectime >= " + boost::lexical_cast<string > (this->start_) +
                "   AND "
                "   rectime <= " + boost::lexical_cast<string > (this->end_);
    }

    return query;
}

bool monitoring_data::get_parameter_specific_(const string &param,
        monitoring_data_result * result) {
    list<string> tables;
    string query_template, query;
    boost::scoped_ptr<soci::rowset<soci::row> > rows;

    this->build_table_names_(&tables);
    query_template = this->build_query_template_();

    for (list<string>::const_iterator it = tables.begin();
            it != tables.end(); ++it) {
        query = (boost::format(query_template) % param % *it).str();

        rows.reset(this->conn_->get_rows(query));
        for (soci::rowset<soci::row>::const_iterator it = rows->begin();
                it != rows->end(); ++it) {
            try {
                result->add_record(
                        boost::posix_time::from_time_t(it->get<long long>(0)),
                        it->get<long long>(1));
            } catch (std::bad_cast) {
                try {
                    result->add_record(
                            boost::posix_time::from_time_t(it->get<long long>(0)),
                            it->get<double>(1));
                } catch (std::bad_cast) {
                    try {
                        // This is for AVG
                        result->add_record(
                                boost::posix_time::from_time_t(it->get<double>(0)),
                                it->get<double>(1));
                    } catch (std::bad_cast) {
                        // This is for SUM
                        result->add_record(
                                boost::posix_time::from_time_t(it->get<double>(0)),
                                it->get<long long>(1));

                    }
                }
            }
        }

    }

    return true;
}

void monitoring_data::build_table_names_(list<string> *tables) {
    // internal_* and physical_* should overwrite this function
    return;
}

monitoring_data::~monitoring_data() {
    if (NULL != this->conn_) {
        delete this->conn_;
    }
}

// -------------- Start internal_monitoring_data -----------------

internal_monitoring_data::internal_monitoring_data(libconfig::Config * config) :
        monitoring_data(config) {
}

internal_monitoring_data::~internal_monitoring_data() {
}

void internal_monitoring_data::set_operation_type(int op_type) {
    this->op_type_ = op_type;
}

void internal_monitoring_data::build_table_names_(list<string> *tables) {
    string table_name = monitoring_const::TABLENAME_INTERNAL +
            monitoring_const::TABLENAME_SEPARATOR;
    if (monitoring_const::INTERNAL_TYPE_READ == this->op_type_) {
        table_name += monitoring_const::TABLENAME_READ +
                monitoring_const::TABLENAME_SEPARATOR;
    } else if (monitoring_const::INTERNAL_TYPE_WRITE == this->op_type_) {
        table_name += monitoring_const::TABLENAME_WRITE +
                monitoring_const::TABLENAME_SEPARATOR;
    }

    if (this->targets_.size() > 0) {
        // Read specific tables
        for (list<string>::const_iterator it = this->targets_.begin();
                it != this->targets_.end(); ++it) {
            if (monitoring_const::INTERNAL_TYPE_BOTH == this->op_type_) {
                tables->push_back(table_name + monitoring_const::TABLENAME_READ +
                        monitoring_const::TABLENAME_SEPARATOR + *it);
                tables->push_back(table_name + monitoring_const::TABLENAME_WRITE +
                        monitoring_const::TABLENAME_SEPARATOR + *it);
            } else {
                tables->push_back(table_name + *it);
            }
        }
    } else {
        // Read all
        string query =
                "SELECT "
                "   text(relname) "
                "FROM "
                "   pg_stat_user_tables "
                "WHERE "
                "   relname ~ '" + table_name + "*'";

        boost::scoped_ptr<soci::rowset<soci::row> > rows(
                this->conn_->get_rows(query));
        for (soci::rowset<soci::row>::const_iterator it = rows->begin();
                it != rows->end(); ++it) {
            tables->push_back(it->get<string > (0));
        }
    }
}

// -------------- Start physical_monitoring_data -----------------

physical_monitoring_data::physical_monitoring_data(libconfig::Config * config) :
        monitoring_data(config) {
}

void physical_monitoring_data::build_table_names_(list<string> *tables) {
    string table_name = monitoring_const::TABLENAME_PHYSICAL +
            monitoring_const::TABLENAME_SEPARATOR;

    if (this->targets_.size() > 0) {
        // Read specific tables
        for (list<string>::const_iterator it = this->targets_.begin();
                it != this->targets_.end(); ++it) {
            tables->push_back(table_name + *it);
        }
    } else {
        // Read all
        string query =
                "SELECT "
                "   text(relname) "
                "FROM "
                "   pg_stat_user_tables "
                "WHERE "
                "   relname ~ '" + table_name + "*'";

        boost::scoped_ptr<soci::rowset<soci::row> > rows(
                this->conn_->get_rows(query));
        for (soci::rowset<soci::row>::const_iterator it = rows->begin();
                it != rows->end(); ++it) {
            tables->push_back(it->get<string > (0));
        }
    }
}

physical_monitoring_data::~physical_monitoring_data() {

}

// -------------- Start monitoring_result -----------------

monitoring_data_result::monitoring_data_result() {
    this->records_ = new list<pair<boost::posix_time::ptime, double>* >();
}

void monitoring_data_result::add_record(
        boost::posix_time::ptime rectime, double value) {
    pair<boost::posix_time::ptime, double> *record =
            new pair<boost::posix_time::ptime, double>(rectime, value);
    this->records_->push_back(record);
}

monitoring_data_result::~monitoring_data_result() {
    list<pair<boost::posix_time::ptime, double>* >::iterator it;
    for (it = this->records_->begin(); it != this->records_->end(); ++it) {
        delete *it;
    }
    delete this->records_;
}