#ifndef ELEMENT
#define ELEMENT

#include<string>

namespace Element {

extern const unsigned char count;
extern const char* symbol[];
extern const char* name[] ;
extern const double mass[] ;
extern const double vdwRadius[];
extern const double covalentRadius[] ;
extern const unsigned char cpkColorRgb[][3] ;
extern const float color[][4] ;

extern int atomicNumber(const std::string & sym);
}



#endif // ELEMENT

