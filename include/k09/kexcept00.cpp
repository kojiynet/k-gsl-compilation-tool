
/*
	kexcept00.cpp
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2014-2019 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	Library for Exception Handling
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kexcept_cpp_include_guard
#define kexcept_cpp_include_guard

#include <exception>
#include <string>


/* ********** Using Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class MsgExcept;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

void throwMsgExcept( const std::string &);
void throwMsgExcept( const std::string &, const std::string &);


/* ********** Class Definitions ********** */

// exception class with message
class MsgExcept : public std::exception {
	
private:
	
	std::string msg;
	
public:
	
	MsgExcept( const std::string &);
	virtual ~MsgExcept( void) throw();
	
	virtual const char * what( void) const throw();
	
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

// throw exception with message indicating the place where error occurred
void throwMsgExcept( const std::string &where0)
{
	
	std::string str( "Error occurred in ");
	str += where0 + ".";
	throw MsgExcept( str);
	
}

// throw exception with message "where0" indicating the place where error occurred,
// and message "what0" indicating what occurred
void throwMsgExcept( const std::string &where0, const std::string &what0)
{
	
	std::string str( "Error occurred in ");
	str += where0 + "; " + what0;
	throw MsgExcept( str);
	
}


/* ********** Definitions of Member Functions ********** */

MsgExcept :: MsgExcept( const std::string &s0)
: msg( s0)
{
}

MsgExcept :: ~MsgExcept( void) throw()
{
}

const char * MsgExcept :: what( void) const throw()
{
	
	return msg.c_str();
	
}


#endif /* kexcept_cpp_include_guard */
