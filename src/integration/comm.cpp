/* 
 * File:   comm.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 * 
 * Created on June 22, 2010, 10:32 AM
 */

#include "common/structures.hpp"
#include "constants.hpp"
#include "pmanager/publisher.hpp"
#include <set>

#include "comm.hpp"

template <class T> static void rpc_cb_serialized(bool &res, T &output,
        const rpcreturn_t &error, const rpcvector_t &result) {
    if (error == rpcstatus::ok && result.size() == 1 &&
            result[0].getValue(&output, true)) {
        res = true;
        return;
    }

    res = false;
    ERROR("could not perform RPC successfully; RPC status is: " << error);
}



comm::comm(libconfig::Config *config) {
    libconfig::Config cfg;

    // get other parameters
    if (!config->lookupValue(
            MERGE_PATH(config_const::SECTION_PMANAGER,
            config_const::PMANAGER_HOST), host_) ||

            !config->lookupValue(
            MERGE_PATH(config_const::SECTION_PMANAGER,
            config_const::PMANAGER_SERVICE), service_)) {
        throw std::runtime_error("pmanager host and/or"
                " service setting not found/invalid");
    }
    rpc_ = new rpc_client_t(io_service_);
}

list<string> comm::get_all_providers() {
    set<string> providers;
    rpcvector_t params;
    bool result;
    metadata::replica_list_t adv;

    rpc_->dispatch(host_, service_, PUBLISHER_GET_ALL_PROVIDERS, params,
            boost::bind(&rpc_cb_serialized<metadata::replica_list_t>,
            boost::ref(result), boost::ref(adv), _1, _2));
    rpc_->run();

    if (!result) {
        return list<string>();
    }

    for (unsigned int i = 0; i < adv.size(); ++i) {
        DBG("Received provider " << adv[i].host << ":" << adv[i].service);
        providers.insert(adv[i].host);
    }

    return list<string>(providers.begin(), providers.end());
}

comm::~comm() {
    delete rpc_;
}

