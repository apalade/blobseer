/* 
 * File:   mover.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 * 
 * Created on June 10, 2010, 3:55 PM
 */

#include "common/debug.hpp"
#include "constants.hpp"
#include <cstdlib>
#include "exceptions.hpp"

#include "mover.hpp"

mover::mover(libconfig::Config *config) : config_(config) {
    const string scripts = MERGE_PATH(config_const::SECTION_MODULE,
            config_const::SECTION_SCRIPTS);

    if (!this->config_->lookupValue(MERGE_PATH(scripts,
            config_const::SCRIPTS_START), start_script_) ||
            !this->config_->lookupValue(MERGE_PATH(scripts,
            config_const::SCRIPTS_STOP), stop_script_)) {
        throw dd_exception("The values of thresholds (add and/or remove) "
                "are not specified.");
    }

}

mover::~mover() {
}

bool mover::start(int number) {
    // TODO: to implement starting a number of providers
    return true;
}

bool mover::stop(string provider) {
    string command = stop_script_ + " " + provider;
    DBG("Running command " << command);
    return system(command.c_str()) == 0;
}

bool mover::stop(list<string> providers) {
    bool ok = true;
    for (list<string>::const_iterator it = providers.begin();
            it != providers.end(); ++it) {
        ok = ok && stop(*it);
    }
    return ok;
}


