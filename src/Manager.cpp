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

#include "Manager.h"

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
Manager::Manager( const string &name ) : 
	Atomic( name ),
	// input ports
	in(addInputPort( "in" )),
	//output ports
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
	this->function=0; //0 mean open for a bit more, 1 means more workers
	this->threshold=5;
	this->isEmergencyData = false;
}

/*******************************************************************
* Function Name: act
* Description: decide what to send to the gates to act in an emergency
********************************************************************/
int Manager::act() {
	return this->function;
}

/*******************************************************************
* Function Name: isEmergency
* Description: decide if input throughput is an emergency
********************************************************************/
bool Manager::isEmergency(Real throughput){
	if (throughput > this->threshold){
		this->isEmergencyData = true;
	}else{
		this->isEmergencyData = false;
	}
	return this->isEmergencyData;
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Manager::initFunction()
{
	gates.reset();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
* 				In case a gate is done, notify Queue and assign next 
********************************************************************/
Model &Manager::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )        // If a new vehicle is ready to be served 
	{
		if (isEmergency(Real::from_value(msg.value()))){ // if there is an emergency
			holdIn( AtomicState::active, preparationTime ); // program to act on gates
		}
	}
	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &Manager::internalFunction( const InternalMessage &msg )
{
	passivate();
	return *this;
}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &Manager::outputFunction( const CollectMessage &msg )
{	
	if (this->isEmergencyData){
		Tuple<Real> out_value{act()};
		sendOutput( msg.time(), out1, out_value) ;
		sendOutput( msg.time(), out2, out_value) ;
		sendOutput( msg.time(), out3, out_value) ;
		sendOutput( msg.time(), out4, out_value) ;
		sendOutput( msg.time(), out5, out_value) ;
		sendOutput( msg.time(), out6, out_value) ;
		sendOutput( msg.time(), out7, out_value) ;
		sendOutput( msg.time(), out8, out_value) ;
		sendOutput( msg.time(), out9, out_value) ;
		sendOutput( msg.time(), out10, out_value) ;
	}
	return *this;

}