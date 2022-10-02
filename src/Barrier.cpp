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

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
* 				In case a gate is done, notify Queue and assign next 
********************************************************************/
Model &Barrier::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )        // If a new vehicle is ready to be served 
	{
		this->next = Real::from_value(msg.value()); // TODO: check if this value is not null
		this->gateAssigned = assignGate();
		if (gateAssigned > -1){ // if there is an available gate
			holdIn( AtomicState::active, preparationTime ); // program to output
		}
	}

	if( msg.port() == done1 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 0;
		}else{
			this->gates[0] = false;
			passivate();
		}
	}
	if( msg.port() == done2 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[1] = false;
			passivate();
		}
	}
	if( msg.port() == done3 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[2] = false;
			passivate();
		}
	}
	if( msg.port() == done4 )      // If a gate is done
	{	
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[3] = false;
			passivate();
		}
	}
	if( msg.port() == done5 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[4] = false;
			passivate();
		}
	}
	if( msg.port() == done6 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[5] = false;
			passivate();
		}
	}
	if( msg.port() == done7 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[6] = false;
			passivate();
		}
	}
	if( msg.port() == done8 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[7] = false;
			passivate();
		}
	}
	if( msg.port() == done9 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[8] = false;
			passivate();
		}
	}
	if( msg.port() == done10 )      // If a gate is done
	{
		if (this->next >= 0){
			holdIn( AtomicState::active, preparationTime ); // program to output
			this->gateAssigned = 1;
		}else{
			this->gates[9] = false;
			passivate();
		}
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
	//Tuple<Real> out_value{Real(1), 0, 1};
	Real out_value = Real::from_value(this->next);
	if( this->gateAssigned == 0 )      //assigned gate
	{
		sendOutput( msg.time(), out1, out_value) ;
	}
	if( this->gateAssigned == 1 )      //assigned gate
	{
		sendOutput( msg.time(), out2, out_value) ;
	}
	if( this->gateAssigned == 2 )      //assigned gate
	{
		sendOutput( msg.time(), out3, out_value) ;
	}
	if( this->gateAssigned == 3 )      //assigned gate
	{
		sendOutput( msg.time(), out4, out_value) ;
	}
	if( this->gateAssigned == 4 )      //assigned gate
	{
		sendOutput( msg.time(), out5, out_value) ;
	}
	if( this->gateAssigned == 5 )      //assigned gate
	{
		sendOutput( msg.time(), out6, out_value) ;
	}
	if( this->gateAssigned == 6 )      //assigned gate
	{
		sendOutput( msg.time(), out7, out_value) ;
	}
	if( this->gateAssigned == 7 )      //assigned gate
	{
		sendOutput( msg.time(), out8, out_value) ;
	}
	if( this->gateAssigned == 8 )      //assigned gate
	{
		sendOutput( msg.time(), out9, out_value) ;
	}
	if( this->gateAssigned == 9 )      //assigned gate
	{
		sendOutput( msg.time(), out10, out_value) ;
	}
	sendOutput( msg.time(), done, out_value) ;
	this->next = -1;
	return *this;

}