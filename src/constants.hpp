/* 
 * File:   constants.h
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on May 11, 2010, 11:50 AM
 */

#ifndef _CONSTANTS_H
#define	_CONSTANTS_H

#include <string>

#define MERGE_PATH(base, last) string(base + "." + last)
#define MERGE_PATH3(base, previous, last) \
    MERGE_PATH(MERGE_PATH(base, previous), last)

namespace config_const {
    const static std::string SECTION_PMANAGER = "pmanager";
    const static std::string PMANAGER_HOST = "host";
    const static std::string PMANAGER_SERVICE = "service";

    const static std::string SECTION_MODULE = "dd";
    const static std::string SECTION_REPO = "repo";
    const static std::string SECTION_DB = "db";
    const static std::string SECTION_DB_DRIVER = "driver";
    const static std::string SECTION_DB_NAME = "name";
    const static std::string SECTION_DB_HOST = "host";
    const static std::string SECTION_DB_USER = "user";
    const static std::string SECTION_DB_PASS = "pass";
    const static std::string SECTION_DB_PORT = "port";

    // Threshold constants
    const static std::string SECTION_THRESHOLD = "thresholds";
    const static std::string SECTION_THRESHOLD_SINGLE = "single";
    const static std::string SECTION_THRESHOLD_AGG = "agg";
    const static std::string THRESHOLD_ADD = "add";
    const static std::string THRESHOLD_REMOVE = "remove";

    // Specifications constants
    const static std::string SECTION_SPEC = "specifications";
    const static std::string SPEC_TARGETS = "targets";
    const static std::string SPEC_TARGETS_ALL = "all";
    const static std::string SPEC_TARGETS_EVERY = "every";
    const static std::string SPEC_INTERVALS = "intervals";
    const static std::string SPEC_CONDS = "conditions";
    const static std::string SPEC_CONDS_UPPER_LIMIT = "upper";
    const static std::string SPEC_CONDS_LOWER_LIMIT = "lower";
    const static std::string SPEC_CONDS_WEIGHT = "weight";
    const static std::string SPEC_CONDS_IMPL = "implementation";
    const static std::string SPEC_FACTORS = "factors";
    const static std::string SPEC_FACTORS_CONDS = "conditions";
    const static std::string SPEC_FACTORS_INTERVAL = "interval";
    const static std::string SPEC_FACTORS_NAME = "name";
    const static std::string SPEC_FACTORS_WEIGHT = "weight";
    const static std::string SPEC_FACTORS_IMPL = "implementation";
    const static std::string SPEC_FACTORS_TYPE = "type";
    const static std::string SPEC_FACTORS_TYPE_PHYSICAL = "physical";
    const static std::string SPEC_FACTORS_TYPE_INTERNAL = "internal";
    const static std::string SPEC_SCENARIOS = "scenarios";
    const static std::string SPEC_SCENARIOS_FACTORS = "factors";
    const static std::string SPEC_SCENARIOS_TARGET = "target";

    // Scripts constants
    const static std::string SECTION_SCRIPTS = "scripts";
    const static std::string SCRIPTS_START = "start";
    const static std::string SCRIPTS_STOP = "stop";
    
    // Misc constants
    const static std::string SLEEP_NO_ACTIVE = "sleep_no_active_providers";
    const static std::string SLEEP_BETWEEN = "sleep_between_cycles";
    const static std::string MAX_CYCLES = "max_cycles";
};

namespace monitoring_const {
    const static int REPO_INTERNAL = 0;
    const static int REPO_PHYSICAL = 1;

    const static int AGGREGATE_NONE = 0;
    const static int AGGREGATE_AVG = 1;
    const static int AGGREGATE_SUM = 2;

    const static int INTERNAL_TYPE_READ = 0;
    const static int INTERNAL_TYPE_WRITE = 1;
    const static int INTERNAL_TYPE_BOTH = 2;

    const static std::string TABLENAME_INTERNAL = "internal";
    const static std::string TABLENAME_PHYSICAL = "physical";
    const static std::string TABLENAME_READ = "read";
    const static std::string TABLENAME_WRITE = "write";
    const static std::string TABLENAME_SEPARATOR = "_";
};

namespace scenario_const {
    const static std::string FACTOR_IMPL_GENERAL = "factor_general";
    const static std::string FACTOR_IMPL_READ = "factor_read";
    const static std::string FACTOR_IMPL_WRITE = "factor_write";
    const static std::string CONDITION_IMPL_GENERAL = "condition_general";
    const static std::string CONDITION_IMPL_RWS = "condition_rws";
}


#endif	/* _CONSTANTS_H */

