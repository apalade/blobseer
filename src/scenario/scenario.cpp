/* 
 * File:   scenario.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 * 
 * Created on June 9, 2010, 10:22 AM
 */


#include <boost/scoped_ptr.hpp>
#include <libconfig.h++>

#include <common/debug.hpp>
#include <exceptions.hpp>
#include <monitoring/monitoring_data.hpp>

#include "scenario.hpp"


// --------------- Begin time_interval --------------------------

time_interval::time_interval() :
        begin_(boost::posix_time::not_a_date_time),
        end_(boost::posix_time::not_a_date_time) {
}

time_interval::~time_interval() {
}

void time_interval::set_begin(int curr) {
    if (CURRENT_TIME != curr) {
        throw dd_exception("Integer set_begin can only "
                "be used with CURRENT_TIME");
    }
    begin_ = boost::posix_time::ptime(
            boost::posix_time::second_clock::local_time());
}

void time_interval::set_end(int curr) {
    if (CURRENT_TIME != curr) {
        throw dd_exception("Integer set_end can only "
                "be used with CURRENT_TIME");
    }
    end_ = boost::posix_time::ptime(
            boost::posix_time::second_clock::local_time());
}

void time_interval::set_begin(boost::posix_time::ptime begin) {
    if (begin.is_not_a_date_time()) {
        throw dd_exception("Begin time was not initialized!");
    }
    begin_ = begin;
}

boost::posix_time::ptime time_interval::get_begin() {
    if (begin_.is_not_a_date_time()) {
        throw dd_exception("Begin time was not initialized!");
    }
    return begin_;
}

void time_interval::set_end(boost::posix_time::ptime end) {
    if (end.is_not_a_date_time()) {
        throw dd_exception("Begin time was not initialized!");
    }

    end_ = end;
}

boost::posix_time::ptime time_interval::get_end() {
    if (end_.is_not_a_date_time()) {
        throw dd_exception("Begin time was not initialized!");
    }

    return end_;
}

string time_interval::to_string() {
    string s = "";
    s += boost::posix_time::to_simple_string(begin_) + " <--> " +
            boost::posix_time::to_simple_string(end_);
    return s;
}

// --------------- Begin target --------------------------

target::target() : all_(false), every_(false) {

}

target::~target() {

}

string target::to_string() {
    string s = "";

    if (all_) {
        return "ALL";
    }

    for (list<string>::const_iterator it = providers_.begin();
            it != providers_.end(); ++it) {
        s += "(" + *it + ")";
    }

    if (every_) {
        s += "[EVERY]";
    }

    return s;
}

void target::add_provider(string &provider) {
    if ("" == provider) {
        INFO("Provider empty, ignoring...");
        return;
    }
    providers_.push_back(provider);
}

// --------------- Begin condition --------------------------

condition::condition() : weight_(0.), name_("UNINTIALIZED CONDITION NAME"),
        lower_(0.), upper_(0.) {
}

condition::~condition() {
}

void condition::set_weight(weight_t weight) {
    if (weight < 0 || weight > 1) {
        throw dd_exception("The weight must have a value between 0 and 1");
    }

    weight_ = weight;
}

string condition::to_string() {
    string s = "";
    s += "Condition '" + name_
            + "'(l: " + boost::lexical_cast<string > (lower_)
            + "; u: " + boost::lexical_cast<string > (upper_)
            + "; w: " + boost::lexical_cast<string > (weight_) + ")";
    return s;
}


// --------------- Begin factor --------------------------

factor::factor() : weight_(.0), name_("UNINTIALIZED FACTOR NAME"),
        config_(NULL) {
}

factor::~factor() {
}

void factor::set_repository(int repo) {
    this->repo_type_ = repo;
}

void factor::set_weight(weight_t weight) {
    if (weight < 0 || weight > 1) {
        throw dd_exception("The weight must have a value between 0 and 1");
    }

    weight_ = weight;
}

void factor::set_config(libconfig::Config *config) {
    if (NULL == config) {
        ERROR("Trying to set configuration file to NULL");
        return;
    }
    config_ = config;
}

string factor::to_string() {
    string s = "";
    s += "Factor '" + name_
            + "'(w: " + boost::lexical_cast<string > (weight_) +
            +"; repo_type: " + boost::lexical_cast<string > (repo_type_)
            + "; interval: " + interval_.to_string() + "; "
            + "no conditions: "
            + boost::lexical_cast<string > (conditions_.size()) + ")";
    return s;
}



// --------------- Begin scenario --------------------------

scenario::scenario(libconfig::Config *config) :
        total_weight_(0.), config_(config) {
}

scenario::~scenario() {
}

void scenario::add_factor(factor *factor) {
    // Set configuration file for the factor
    factor->set_config(config_);

    // Push it in our list
    factors_.push_back(factor);

    // Check the total weight of the factors in this scenario
    total_weight_ += factor->get_weight();
    if (total_weight_ > 1.) {
        ERROR("The total weight of the factors in this scenario exceeds 100%. "
                "Results may be awkward.");
    }
}

score_t scenario::get_score() {
    double score;
    weight_t condition_weight;
    boost::scoped_ptr<monitoring_data_result> factor_value;
    factor *f;
    condition *c;

    if (total_weight_ < 1.) {
        ERROR("The total weight of the factors in this scenario is less "
                "than 100%. The resulting score may be awkward.");
    }

    score = 0.;
    for (list<factor *>::const_iterator it = factors_.begin();
            it != factors_.end(); ++it) {
        f = *it;

        condition_weight = 0.;
        factor_value.reset(f->get_value(target_));
        for (list<condition *>::const_iterator it2 = f->get_conditions_begin();
                it2 != f->get_conditions_end(); ++it2) {
            c = *it2;

            if (c->is_true(factor_value.get(), f->get_interval())) {
                // Stop at the first true condition
                condition_weight = c->get_weight();
                break;
            }
        }

        // Sum of weight of factor multiplied with the weight of condition
        score += f->get_weight() * condition_weight;
    }

    return score;
}

string scenario::to_string() {
    string s = "";
    s += "( target: " + target_.to_string()
            + "; total weight: " + boost::lexical_cast<string > (total_weight_)
            + "; factors: ";
    for (list<factor *>::const_iterator it = factors_.begin();
            it != factors_.end(); ++it) {
        factor *f = *it;
        s += f->get_name() + " ";
    }
    return s + ")";

}