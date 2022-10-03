/** include files **/
#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "message.h" // class ExternalMessage, InternalMessage
#include "parsimu.h" // ParallelMainSimulator::Instance().getParameter( ... )
#include "real.h"
#include "tuple_value.h"
#include "real.h"
#include "distri.h"        // class Distribution
#include "strutil.h"

#include "Barrier.h"

using namespace std;

#define VERBOSE true

#define PRINT_TIMES(f) {\
	cout << f << "@" << msg.time() <<\
		" - timeleft: " << timeleft <<\
		" - elapsed: " << elapsed <<\
		" - sigma: " << sigma << endl;\
}

/** public functions **/

/*******************************************************************
* Function Name: [#MODEL_NAME#]
* Description: constructor
********************************************************************/
Barrier::Barrier( const string &name ) : 
	Atomic( name ),
	// input ports
	in(addInputPort( "in" )),
	done1(addInputPort( "done1" )),
	done2(addInputPort( "done2" )),
	done3(addInputPort( "done3" )),
	done4(addInputPort( "done4" )),
	done5(addInputPort( "done5" )),
	done6(addInputPort( "done6" )),
	done7(addInputPort( "done7" )),
	done8(addInputPort( "done8" )),
	done9(addInputPort( "done9" )),
	done10(addInputPort( "done10" )),
	//output ports
	done(addOutputPort( "done" )),
	out1(addOutputPort( "out1" )),
	out2(addOutputPort( "out2" )),
	out3(addOutputPort( "out3" )),
	out4(addOutputPort( "out4" )),
	out5(addOutputPort( "out5" )),
	out6(addOutputPort( "out6" )),
	out7(addOutputPort( "out7" )),
	out8(addOutputPort( "out8" )),
	out9(addOutputPort( "out9" )),
	out10(addOutputPort( "out10" )),
	preparationTime( 0, 0, 0, 1 )
{
	this->amountOfGates = 10;
	this->gateAssigned = 0;
	this->empty = Tuple<Real>({Real(-1)});
}

/*******************************************************************
* Function Name: assignGate
* Description: check if a gate is available, occupy it and return that one
********************************************************************/
int Barrier::assignGate() {
	for (std::size_t i = 0; i < this->gates.size(); ++i) {
	    if (!this->gates[i]){
	    	this->gates[i] = true;
	    	return i;
	    }
	}
	return -1; // in case that every gate is beign used
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Barrier::initFunction()
{
	gates.reset();
	return *this ;
}

void Barrier::externalFunctionHelper( const ExternalMessage &msg, int i ){
	if (this->next != this->empty && Real::from_value(msg.value()) == 0){ //tengo next y está disponible la gate
		holdIn( AtomicState::active, preparationTime ); // program to output
		this->gateAssigned = i;
	}else if(Real::from_value(msg.value()) == 1){//no está disponible
		this->gates[i] = true; // marco como no disponible
		passivate();	
	}else{ //esta disponible pero no tengo
		this->gates[i] = false; // marco como disponible
		passivate();
	}
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
* 				In case a gate is done, notify Queue and assign next 
********************************************************************/
Model &Barrier::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )        // If a new vehicle is ready to be served 
	{
		Tuple<Real> truck = Tuple<Real>::from_value(msg.value());
		this->next = truck;
		this->gateAssigned = assignGate();
		if (gateAssigned > -1){ // if there is an available gate
			holdIn( AtomicState::active, preparationTime ); // program to output
		}
	}

	if( msg.port() == done1 )      // If a gate is done
	{
		externalFunctionHelper(msg, 0);
	}
	if( msg.port() == done2 )      // If a gate is done
	{
		externalFunctionHelper(msg, 1);
	}
	if( msg.port() == done3 )      // If a gate is done
	{
		externalFunctionHelper(msg, 2);
	}
	if( msg.port() == done4 )      // If a gate is done
	{	
		externalFunctionHelper(msg, 3);
	}
	if( msg.port() == done5 )      // If a gate is done
	{
		externalFunctionHelper(msg, 4);
	}
	if( msg.port() == done6 )      // If a gate is done
	{
		externalFunctionHelper(msg, 5);
	}
	if( msg.port() == done7 )      // If a gate is done
	{
		externalFunctionHelper(msg, 6);
	}
	if( msg.port() == done8 )      // If a gate is done
	{
		externalFunctionHelper(msg, 7);
	}
	if( msg.port() == done9 )      // If a gate is done
	{
		externalFunctionHelper(msg, 8);
	}
	if( msg.port() == done10 )      // If a gate is done
	{
		externalFunctionHelper(msg, 9);
	}

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &Barrier::internalFunction( const InternalMessage &msg )
{
	passivate();
	return *this;
}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &Barrier::outputFunction( const CollectMessage &msg )
{	
	if( this->gateAssigned == 0 )      //assigned gate
	{
		sendOutput( msg.time(), out1, this->next) ;
	}
	if( this->gateAssigned == 1 )      //assigned gate
	{
		sendOutput( msg.time(), out2, this->next) ;
	}
	if( this->gateAssigned == 2 )      //assigned gate
	{
		sendOutput( msg.time(), out3, this->next) ;
	}
	if( this->gateAssigned == 3 )      //assigned gate
	{
		sendOutput( msg.time(), out4, this->next) ;
	}
	if( this->gateAssigned == 4 )      //assigned gate
	{
		sendOutput( msg.time(), out5, this->next) ;
	}
	if( this->gateAssigned == 5 )      //assigned gate
	{
		sendOutput( msg.time(), out6, this->next) ;
	}
	if( this->gateAssigned == 6 )      //assigned gate
	{
		sendOutput( msg.time(), out7, this->next) ;
	}
	if( this->gateAssigned == 7 )      //assigned gate
	{
		sendOutput( msg.time(), out8, this->next) ;
	}
	if( this->gateAssigned == 8 )      //assigned gate
	{
		sendOutput( msg.time(), out9, this->next) ;
	}
	if( this->gateAssigned == 9 )      //assigned gate
	{
		sendOutput( msg.time(), out10, this->next) ;
	}
	sendOutput( msg.time(), done, this->next[0]) ;
	this->next = this->empty;
	return *this;

}