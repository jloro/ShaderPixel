#ifndef SDLEVENT_HPP
# define SDLEVENT_HPP

# include <iostream>

class SdlEvent {

public: 
/*	constructors / destrucors	*/
    SdlEvent(void); 
    SdlEvent(const SdlEvent & src); 
    ~SdlEvent(void); 

/*	public variables	*/
/*	public functions	*/
	SdlEvent &		operator=(const SdlEvent & rhs);

    std::string const	toString(void) const;

private:
/*	private variables	*/
/*	private variables	*/

};

std::ostream &	operator<< (std::ostream & o, const SdlEvent & rhs);

#endif