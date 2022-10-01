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

#include "Gate.h"

using namespace std;

#define VERBOSE true

#define PRINT_TIMES(f) {\
	cout << f << "@" << msg.time() <<\
		" - timeleft: " << timeLeft <<\
		" - elapsed: " << elapsed <<\
		" - sigma: " << sigma << endl;\
}


/** public functions **/

/*******************************************************************
* Function Name: [#MODEL_NAME#]
* Description: constructor
********************************************************************/
Gate::Gate( const string &name ) : 
	Atomic( name ),
	out(addOutputPort( "out" )),
	out(addOutputPort( "toBarrier" )),
	in(addInputPort( "fromBarrier" ))
{
	startHour = 7;
	endHour = 23;
	dist = Distribution::create("normal");
	MASSERT( dist ) ;
	dist->setVar( 0,0 ) ; // mean
	dist->setVar( 1,1 ) ; // var
	currentState = UNAVAILABLE;
 	currentTruck = NULL;


}


int Gate::workersNow(VTime now) {
	// TODO cambiar para que dependa del momento del dia
	return 5;
}


VTime Gate::proccesingTimeTruck(VTime now, Tuple<Real> truck) {
	int workers = workersNow(now);
	// TODO cambiar
	return VTime(2,0,0,0)

}


VTime Gate::wakeUpSigmaAt(VTime now, int hourOfDay) {
	int hoursToNextAwake = (24 - (current_hours)%24) + hourOfDay;
	VTime targetAwake = VTime(hoursToNextAwake+current_hours,0,0,0) - now;
	return targetAwake;
}


VTime Gate::proccesingTimePacketA() {
	float mean = 60; // un minuto
	float std = 30; // mas menos 30 segundos

	double seconds = 0;
	while (seconds <= 0) {
		double sample = distribution().get() * std + mean;
		seconds = sample;
	}
	return VTime(seconds);

}

VTime Gate::proccesingTimePacketB() {
	float mean = 60; // un minuto
	float std = 30; // mas menos 30 segundos

	double seconds = 0;
	while (seconds <= 0) {
		double sample = distribution().get() * std + mean;
		seconds = sample;
	}
	return VTime(seconds);
}

VTime Gate::proccesingTimePacketC() {
	float mean = 60; // un minuto
	float std = 30; // mas menos 30 segundos

	double seconds = 0;
	while (seconds <= 0) {
		double sample = distribution().get() * std + mean;
		seconds = sample;
	}
	return VTime(seconds);
}
/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Gate::initFunction()
{
	// [(!) Initialize common variables]
	this->elapsed  = VTime::Zero;
	this->sigma    = VTime(startHour,0,0,0); // force an internal transition in t=0;
	this->timeLeft = this->sigma;

 	// set next transition
 	holdIn( AtomicState::active, this->sigma  ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
********************************************************************/
Model &Gate::externalFunction( const ExternalMessage &msg )
{
	Tuple<Real> truck = msg.value()
	currentTruck = truck;
	this->sigma    = proccesingTimeTruck(msg.time(), truck);	
	this->elapsed  = VTime::Zero;
 	this->timeLeft = this->sigma - this->elapsed; 

	currentState = BUSY;

#if VERBOSE
	PRINT_TIMES("dext");
#endif
	
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &Gate::internalFunction( const InternalMessage &msg )
{

	// UNAVAILABLE -> FREE (empezo el dia)
	// BUSY -> FREE (termino truck)
	// BUSY -> UNAVAILABLE (termino truck y se acabo el dia)
	// FREE -> UNAVAILABLE (se acabo el dia)

	if (currentState == UNAVAILABLE) {
		currentTruck = NULL;
		currentState = FREE;
		this->sigma = wakeUpSigmaAt(msg.time(),endHour );
	}

	else if (currentState == BUSY) {
		if (isActivePeriod(msg.time())) {
			currentState = FREE;
			this->sigma = wakeUpSigmaAt(msg.time(),endHour );
		}
		else {
			currentState = UNAVAILABLE;
			this->sigma = wakeUpSigmaAt(msg.time(),startHour );
		}
	}

	else if (currentState == FREE ) {
		currentTruck = NULL;
		currentState = UNAVAILABLE;
		this->sigma =  wakeUpSigmaAt(msg.time(),startHour );

	}

	this->elapsed  = VTime::Zero;
 	this->timeLeft = this->sigma - this->elapsed; 

	holdIn( AtomicState::active, this->sigma );
	return *this;

}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &Gate::outputFunction( const CollectMessage &msg )
{
	// convencion para Barrier
	// 0: estoy disponible
	// 1: no estoy disponible

	if (currentState == FREE) {
		sendOutput( msg.time(), toBarrier, 0) ;
		
	}
	else {
		sendOutput( msg.time(), toBarrier, 1) ;
	}


	// si termino un camion lo outputeo
	if ( (currentState == FREE or currentState == UNAVAILABLE) and currentTruck != NULL) {
		sendOutput( msg.time(), out, currentTruck) ;
	}
	return *this;

}

Gate::~Gate()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
