/* 
 * File:   exceptions.cpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 * 
 * Created on June 9, 2010, 12:09 PM
 */

#include "exceptions.hpp"

dd_exception::dd_exception(string ex) {
    ex_ = ex;
}

const char* dd_exception::what() const throw () {
    return ex_.c_str();
}

dd_exception::~dd_exception() throw () {

}

