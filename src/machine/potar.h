#ifndef POTAR_H
#define POTAR_H



#include "Connect.h"
#include "pobject.h"


class DialogPotar;

class Cpotar:public CPObject{
public:
    const char*	GetClassName(){ return("Cpotar");}

    Cconnector	*pADCONNECTOR;

    bool run(void);
    bool init(void);				//initialize
    bool exit(void);				//end

    BYTE get_value(void);
    void set_value(BYTE);

    Cpotar(CPObject *parent = 0);
    virtual ~Cpotar();

protected:
    void paintEvent(QPaintEvent *);

private:
    // Analogic value
    BYTE value;

    DialogPotar *dialogpotar;
};


#endif // POTAR_H
