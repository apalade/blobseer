/* 
 * File:   poolmanager.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 * 
 * Created on June 10, 2010, 3:54 PM
 */

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "constants.hpp"
#include "exceptions.hpp"
#include "mover.hpp"
#include "scenario/scenario.hpp"
#include "scenario/implementation.hpp"

#include "poolmanager.hpp"
#define __DEBUG
#include <common/debug.hpp>

poolmanager::poolmanager(libconfig::Config *config) : config_(config),
        add_threshold_(-1), remove_threshold_(-1), sleep_no_active_(0),
        sleep_between_(0), max_cycles_(0), comm_(config) {
    monitoring_data_ = new monitoring_data(config_);
    mover_ = new mover(config_);

    INFO("Initializing thresholds");
    init_thresholds_();
    INFO("Initializing specifications");
    init_specifications_();
    INFO("Initializing misc options");
    init_misc_();
}

poolmanager::~poolmanager() {
    delete monitoring_data_;
    delete mover_;

    BOOST_FOREACH(scenarios_map_t::value_type i, scenarios_by_name_) {
        delete i.second;
    }

    BOOST_FOREACH(factors_map_t::value_type i, factors_by_name_) {
        delete i.second;
    }

    BOOST_FOREACH(conditions_map_t::value_type i, conds_by_name_) {
        delete i.second;
    }

    BOOST_FOREACH(targets_map_t::value_type i, targets_by_name_) {
        delete i.second;
    }

    BOOST_FOREACH(intervals_map_t::value_type i, intervals_by_name_) {
        delete i.second;
    }
}

void poolmanager::run() {
    list<string> providers;
    score_t score;
    int cycle_no;

    cycle_no = 1;
    do {
        score = 0;
        INFO("Starting cycle " << cycle_no << "/" << max_cycles_);
        INFO("Getting active providers");
        providers = get_active_providers_();
        if (providers.size() < 1) {
            INFO("No active providers, waiting for " << sleep_no_active_ << ""
                    " seconds before retrying... ");
            sleep(sleep_no_active_);
            continue;
        }

        INFO("Analyzing pool");

        BOOST_FOREACH(scenarios_map_t::value_type it, scenarios_by_name_) {
            scenario *s = it.second;
            target t = s->get_target();

            if (t.isEvery()) {
                // Modify the target depending on the actual running
                // providers in the system

                BOOST_FOREACH(string provider, providers) {
                    t.reset_providers();
                    t.add_provider(provider);
                    s->set_target(t);
                    action_(s);
                }
            } else {
                action_(s);
            }
        }

        // Stop condition
        ++cycle_no;
        if (cycle_no <= max_cycles_) {
            INFO("Sleeping " << sleep_between_ << " seconds before "
                    "checking again.");
            sleep(sleep_between_);
        } else {
            INFO("Job done.");
            break;
        }

    } while (true);
    INFO("Exiting...");
}

list<string> poolmanager::get_active_providers_() {
    return comm_.get_all_providers();
}

void poolmanager::get_critical_providers_(
        boost::unordered_set<string> *critical_providers) {
    monitoring_data_->get_critical_providers(critical_providers);
    DBG("Computed critical providers to: ");
    for (boost::unordered_set<string>::const_iterator it = critical_providers->begin();
            it != critical_providers->end(); ++it) {
        DBG(*it);
    }
}

void poolmanager::action_(scenario *s) {
    double score;
    target t;
    list<string> providers;
    list<string>::iterator provider_it;
    boost::unordered_set<string> critical_providers;
    bool ok;

    t = s->get_target();
    providers = t.get_providers();
    get_critical_providers_(&critical_providers);

    INFO("Analyzing target: " << t.to_string());
    INFO("Removing critical providers from the list");
    provider_it = providers.begin();
    while (provider_it != providers.end()) {
        if (critical_providers.find(*provider_it) != critical_providers.end()) {
            // We have it as a critical provider, remove it from targets
            DBG("Removing critical provider " << *provider_it);
            provider_it = providers.erase(provider_it);
        } else {
            ++provider_it;
        }
    }
    if (providers.size() < 1) {
        INFO("No action taken - no non-critical providers to be removed!");
        return;
    }


    score = s->get_score();
    DBG("A score of " + boost::lexical_cast<string > (score) + " has been "
            "computed for target " + t.to_string());
    if (score <= add_threshold_) {
        INFO("Adding providers");
        ok = mover_->start(0);
    } else if (score >= remove_threshold_) {
        INFO("Removing providers @ " << t.to_string());
        ok = mover_->stop(t.get_providers());
    }
    if (ok) {
        INFO("Successful operation.");
    } else {
        INFO("Unsuccessful operation. Ignoring.");
    }
}

void poolmanager::init_thresholds_() {
    const string threshold_path = MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SECTION_THRESHOLD);

    if (!this->config_->lookupValue(MERGE_PATH(threshold_path,
            config_const::THRESHOLD_ADD), add_threshold_) ||
            !this->config_->lookupValue(MERGE_PATH(threshold_path,
            config_const::THRESHOLD_REMOVE), remove_threshold_)) {
        throw dd_exception("The values of thresholds (add and/or remove) "
                "are not specified.");
    }

    if (add_threshold_ < 0 || add_threshold_ > 1) {
        throw dd_exception("The values of add threshold must be a real number "
                "between 0 and 1");
    }

    if (remove_threshold_ < 0 || remove_threshold_ > 1) {
        throw dd_exception("The values of remove threshold must be a real "
                "number between 0 and 1");
    }
}

void poolmanager::init_misc_() {
    if (!this->config_->lookupValue(MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SLEEP_NO_ACTIVE), sleep_no_active_) ||
            !this->config_->lookupValue(MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SLEEP_BETWEEN), sleep_between_) ||
            !this->config_->lookupValue(MERGE_PATH(config_const::SECTION_MODULE,
            config_const::MAX_CYCLES), max_cycles_)
            ) {
        throw dd_exception("Values for sleeping/max_cycles "
                "from configuration file are not correct.");
    }
}

void poolmanager::init_specifications_() {
    // Iterate throught specifications and build them
    init_targets_();
    init_time_intervals_();
    init_conditions_();
    init_factors_();
    init_scenarios_();
}

void poolmanager::init_targets_() {
    const string specs_path = MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SECTION_SPEC);
    const string specs_targets_path = MERGE_PATH(specs_path,
            config_const::SPEC_TARGETS);

    libconfig::Setting &setting = config_->lookup(specs_targets_path);
    for (int i = 0; i < setting.getLength(); ++i) {
        string name = setting[i].getName();
        string value = setting[i];

        target *t = new target();
        if (value == config_const::SPEC_TARGETS_ALL) {
            t->enableAll();
        } else if (value == config_const::SPEC_TARGETS_EVERY) {
            t->enableEvery();
        } else {
            t->add_provider(value);
        }
        targets_by_name_[name] = t;
    }
}

void poolmanager::init_time_intervals_() {
    const string specs_path = MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SECTION_SPEC);
    const string specs_targets_path = MERGE_PATH(specs_path,
            config_const::SPEC_INTERVALS);

    libconfig::Setting &setting = config_->lookup(specs_targets_path);
    for (int i = 0; i < setting.getLength(); ++i) {
        string name = setting[i].getName();
        libconfig::Setting &value = setting[i];

        time_interval *t = new time_interval();
        switch (value.getLength()) {
            case 2:
                t->set_begin(normalize_time_(value[0]));
                t->set_end(normalize_time_(value[1]));
                break;
            case 1:
                t->set_begin(normalize_time_(value[0]));
                t->set_end(time_interval::CURRENT_TIME);
                break;
            default:
                ERROR("Wrong time interval in configuration file");
        }

        intervals_by_name_[name] = t;
        DBG("Found interval '" << name << "': " << t->to_string());
    }
}

void poolmanager::init_conditions_() {
    const string specs_path = MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SECTION_SPEC);
    const string specs_conds_path = MERGE_PATH(specs_path,
            config_const::SPEC_CONDS);

    libconfig::Setting &setting = config_->lookup(specs_conds_path);
    for (int i = 0; i < setting.getLength(); ++i) {
        string name = setting[i].getName();

        // TODO: Refactor the following allocation to use the
        // factory pattern in order to hide the implementation details
        condition *cond;
        string cls = setting[i][config_const::SPEC_FACTORS_IMPL];
        if (cls == scenario_const::CONDITION_IMPL_GENERAL) {
            cond = new condition_general();
        } else if (cls == scenario_const::CONDITION_IMPL_RWS) {
            cond = new condition_rws();
        } else {
            ERROR("There is no " << cls << " class implementation");
            throw dd_exception("No " + cls + " implementation");
        }

        cond->set_upper_limit(setting[i][config_const::SPEC_CONDS_UPPER_LIMIT]);
        cond->set_lower_limit(setting[i][config_const::SPEC_CONDS_LOWER_LIMIT]);
        cond->set_weight(setting[i][config_const::SPEC_CONDS_WEIGHT]);
        cond->set_name(name);
        conds_by_name_[name] = cond;

        DBG("Found condition: " << cond->to_string());
    }
}

void poolmanager::init_factors_() {
    const string specs_path = MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SECTION_SPEC);
    const string specs_factors_path = MERGE_PATH(specs_path,
            config_const::SPEC_FACTORS);

    libconfig::Setting &setting = config_->lookup(specs_factors_path);
    for (int i = 0; i < setting.getLength(); ++i) {
        string name = setting[i].getName();

        // TODO: Refactor the following allocation to use the
        // factory pattern in order to hide the implementation details
        factor *f;
        string cls = setting[i][config_const::SPEC_FACTORS_IMPL];
        if (cls == scenario_const::FACTOR_IMPL_GENERAL) {
            f = new factor_general();
        } else if (cls == scenario_const::FACTOR_IMPL_READ) {
            f = new factor_read();
        } else if (cls == scenario_const::FACTOR_IMPL_WRITE) {
            f = new factor_write();
        } else {
            ERROR("There is no " << cls << " class implementation");
            throw dd_exception("No " + cls + " implementation");
        }

        for (int j = 0;
                j < setting[i][config_const::SPEC_FACTORS_CONDS].getLength();
                ++j) {
            f->add_condition(
                    conds_by_name_[setting[i][config_const::SPEC_FACTORS_CONDS][j]]);
        }
        f->set_interval(
                *(intervals_by_name_[setting[i][config_const::SPEC_FACTORS_INTERVAL]]));
        f->set_weight(setting[i][config_const::SPEC_FACTORS_WEIGHT]);
        f->set_name(setting[i][config_const::SPEC_FACTORS_NAME]);

        // Repository in configuration file to repository in monitoring system
        string type = setting[i][config_const::SPEC_FACTORS_TYPE];
        if (type == config_const::SPEC_FACTORS_TYPE_PHYSICAL) {
            f->set_repository(monitoring_const::REPO_PHYSICAL);
        } else if (type == config_const::SPEC_FACTORS_TYPE_INTERNAL) {
            f->set_repository(monitoring_const::REPO_INTERNAL);
        } else {
            ERROR("Type for factor " << f->get_name() << " is wrong");
        }

        // Save factor
        factors_by_name_[name] = f;

        DBG("Found factor (" << name << "): " << f->to_string());
    }
}

void poolmanager::init_scenarios_() {
    const string specs_path = MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SECTION_SPEC);
    const string specs_scenarios_path = MERGE_PATH(specs_path,
            config_const::SPEC_SCENARIOS);

    libconfig::Setting &setting = config_->lookup(specs_scenarios_path);
    for (int i = 0; i < setting.getLength(); ++i) {
        string name = setting[i].getName();

        scenario *s = new scenario(config_);
        for (int j = 0;
                j < setting[i][config_const::SPEC_SCENARIOS_FACTORS].getLength();
                ++j) {
            s->add_factor(
                    factors_by_name_.at(
                    setting[i][config_const::SPEC_SCENARIOS_FACTORS][j]));
        }
        s->set_target(
                *(targets_by_name_.at(
                setting[i][config_const::SPEC_SCENARIOS_TARGET])));
        scenarios_by_name_[name] = s;
        DBG("Found scenario '" << name << "': " << s->to_string());
    }
}

boost::posix_time::ptime poolmanager::normalize_time_(int seconds) {
    boost::posix_time::ptime p = boost::posix_time::ptime(
            boost::posix_time::second_clock::local_time());
    if (-1 == seconds) {
        return p;
    }

    return p - boost::posix_time::seconds(seconds);
}