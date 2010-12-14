/* 
 * File:   mover.hpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on June 10, 2010, 3:55 PM
 */

#ifndef _MOVER_HPP
#define	_MOVER_HPP

#include <libconfig.h++>
#include <list>
#include <string>

using namespace std;

/**
 * The mover class is responsabile to actually start or shutdown providers
 * as indicated by the poolmanager
 */
class mover {
public:
    mover(libconfig::Config *config);
    virtual ~mover();

    /**
     * Stop specified providers by their IP address
     *
     * @ return true on success, false otherwise
     */
    bool stop(string provider);
    bool stop(list<string> providers);

    /**
     * Start a number of providers
     *
     * @return true on success, false otherwise
     */
    bool start(int number);
private:
    libconfig::Config *config_;
    string start_script_, stop_script_;

};

#endif	/* _MOVER_HPP */

