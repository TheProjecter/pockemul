
#include "clink.h"
#include "Log.h"

CDirectLink::CDirectLink(void) {

}

// Return the first connected object found
CPObject * CDirectLink::findObj(CPObject * search)
{
	for (int i = 0;i < search->ConnList.size(); i++)
 	{
        int found = AConnList.indexOf(search->ConnList.at(i));
        if (found >= 0)	return BConnList.at(found)->Parent;
        found = BConnList.indexOf(search->ConnList.at(i));
        if (found >= 0)	return AConnList.at(found)->Parent;

	}
	return 0;
}

// return a list of all connected  objects
void CDirectLink::findAllObj(CPObject * search, QList<CPObject *> * liste)
{
	int i,j;
	
	for (i = 0;i < search->ConnList.size(); i++)
 	{
 		
		for (j = 0;j < AConnList.size(); j++)
	 	{
	 		if ( search->ConnList.at(i) == AConnList.at(j) ) 
	 		{
	 			if (liste->indexOf(BConnList.at(j)->Parent) == -1)
	 			{
	 				liste->append(BConnList.at(j)->Parent);
	 				findAllObj(BConnList.at(j)->Parent, liste);
 				}
 			}
		}
		for ( j = 0;j < BConnList.size(); j++)
	 	{
	 		if ( search->ConnList.at(i) == BConnList.at(j) ) 
	 		{
	 			if (liste->indexOf(AConnList.at(j)->Parent) == -1)
	 			{
	 				liste->append(AConnList.at(j)->Parent);
	 				findAllObj(AConnList.at(j)->Parent, liste);
 				}
 			}
		}
	}

}


bool CDirectLink::isLinked(CPObject * search)
{
	return ( findObj(search) ? true : false );
}


void CDirectLink::Output(CPObject* pPC)
{
	for (int i = 0;i < pPC->ConnList.size(); i++)
 	{
 		outConnector( pPC->ConnList.at(i) );
	}
}

void CDirectLink::outConnector(Cconnector* search)
{
	
	// Search the connector in List A
	int found = AConnList.indexOf(search);
    Cconnector *foundConnector;
    if ( found >= 0 ) {
        foundConnector = BConnList.at(found);
    }
    else { 	// Search the connector in List B
        found = BConnList.indexOf(search);
        if ( found >= 0 ) {
            foundConnector = AConnList.at(found);
        }
    }
    if ( found >= 0 ) {
        foundConnector->ConnectTo(search);

        // start the corresponding pPC
        // if no frequency link to the parent timer
        if (
            (foundConnector->Parent->getfrequency() == 0 ) &&
             (! inlogrun(foundConnector->Parent) ))
        {
//				AddLog(LOG_TEMP,tr("Connector Chain with %1").arg(BConnList.at(found)->Parent->getName()));
            foundConnector->Parent->pTIMER = search->Parent->pTIMER;
            foundConnector->Parent->run();
            insertlog(foundConnector->Parent);
            Output( foundConnector->Parent );
        }
    }
}




bool CLink11to11::run()
{
	
	// DIRECT LINK
//	pSIO->pSIOCONNECTOR->ConnectTo(pPC->pSIOCONNECTOR);	
	
	return true;
}
