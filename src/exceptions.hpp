/* 
 * File:   exceptions.hpp
 * Author: Alexandru Palade <alexandru.palade@loopback.ro>
 *
 * Created on June 9, 2010, 12:09 PM
 */

#ifndef _EXCEPTIONS_HPP
#define	_EXCEPTIONS_HPP

#include <exception>
#include <string>

using namespace std;

class dd_exception : public exception {
public:
    dd_exception(string ex);
    virtual const char* what() const throw ();
    ~dd_exception() throw();

private:
    string ex_;
};

#endif	/* _EXCEPTIONS_HPP */

