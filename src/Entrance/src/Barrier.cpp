/** include files **/
#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "message.h" // class ExternalMessage, InternalMessage
#include "parsimu.h" // ParallelMainSimulator::Instance().getParameter( ... )
#include "real.h"
#include "tuple_value.h"
#include "distri.h"        // class Distribution
#include "strutil.h"

#include "Router.h"

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
	Atomic( name )
	out1(addOutputPort( "out1" ))
	out2(addOutputPort( "out2" ))
	out3(addOutputPort( "out3" ))
	out4(addOutputPort( "out4" ))
	out5(addOutputPort( "out5" ))
	out6(addOutputPort( "out6" ))
	out7(addOutputPort( "out7" ))
	out8(addOutputPort( "out8" ))
	out9(addOutputPort( "out9" ))
	out10(addOutputPort( "out10" ))

{
	
}

/*******************************************************************
* Function Name: assignGate
* Description: check if a gate is available, occupy it and return that one
********************************************************************/
Barrier::assignGate() {
	for (std::size_t i = 0; i < this->gates.size(); ++i) {
	    if (!this->gates[i]){
	    	this-.gates[i] = true;
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
	// [(!) Initialize common variables]
	this->elapsed  = VTime::Zero;
 	this->timeLeft = VTime::Inf;
 	// this->sigma = VTime::Inf; // stays in active state until an external event occurs;
 	this->sigma    = VTime::Zero; // force an internal transition in t=0;

 	// TODO: add init code here. (setting first state, etc)
 	
 	// set next transition
 	holdIn( AtomicState::active, this->sigma  ) ;
	return *this ;
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
		this->next = msg.value(); // TODO: check if this value is not null
		int gateAssigned = assignGate();
		if (gateAssigned > -1){ // if there is an available gate
			holdIn( AtomicState::active, preparationTime ); // program DONE to Queue
		}
	}

	if( msg.port() == done )      // If a gate is done
	{
		elements().pop_front() ;  // Eliminar solicitud actual de cola
		if( !elements().empty() )
			holdIn( AtomicState::active, preparationTime );
			// Programar siguiente envío
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

	//[(!) update common variables]	
	this->sigma    = nextChange();	
	this->elapsed  = msg.time()-lastChange();	
 	this->timeLeft = this->sigma - this->elapsed; 

#if VERBOSE
	PRINT_TIMES("dint");
#endif

	if (scheudledTrucks..size() == 0) {
		scheudleTrucksForTheDay();
		int current_hours = msg.time().hours();
		int hoursToNextAwake =  24 - (current_hours%24) + startHour
		VTime targetAwake = VTime(current_hours+hoursToNextAwake,0,0,0);
		this->sigma = targetAwake-msg.time()
	}
	else {
		this->sigma = scheudledTrucks.pop_front();
	}

	holdIn( AtomicState::active, this->sigma );
	return *this;
}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &Barrier::outputFunction( const CollectMessage &msg )
{
	// TODO cambiar para que escupa camiones
	sendOutput( msg.time(), out, 0) ;
	return *this;

}

Barrier::~Barrier()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
