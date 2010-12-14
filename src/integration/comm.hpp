/* 
 * File:   comm.hpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on June 22, 2010, 10:32 AM
 */

#ifndef _COMM_HPP
#define	_COMM_HPP

#include "common/config.hpp"
#include <libconfig.h++>
#include <list>
#include "rpc/rpc_client.hpp"

using namespace std;

class comm {
public:
    comm(libconfig::Config *config);
    list<string> get_all_providers();
    virtual ~comm();
private:
    typedef rpc_client<config::socket_namespace> rpc_client_t;

    rpc_client_t *rpc_;
    boost::asio::io_service io_service_;
    string host_, service_;
};

#endif	/* _COMM_HPP */

