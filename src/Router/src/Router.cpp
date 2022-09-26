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
Router::Router( const string &name ) : 
	Atomic( name )
	out(addOutputPort( "out" ))
{
	startHour = 7;
	endHour = 11;
	dist = Distribution::create("normal");
	MASSERT( dist ) ;
	dist->setVar( 0,0 ) ; // mean
	dist->setVar( 1,1 ) ; // var
}


Router::scheudleTrucksForTheDay() {
	// TODO cambiar estas tres a parametros ?
	int n = 200; 
	float mean = (endHour+startHour)/2.0;
	float stdev = 1.0;

	while (scheudledTrucks.size() < n) {
		double sample = distribution().get();
		sample = sample*stddev + mean
		
		if (startHour <= sample and sample <= endHour ) {
			sample = sample * 3600
			VTime( static_cast< float >(sample) ) time;
			scheudledTrucks.push_back(time);
		}
	}

	scheudledTrucks.sort();

	// iteramos la lista para obtener la diferencia con el anterior
	VTime prevValue = VTime(startHour,0,0,0);
	auto  it = scheudledTrucks.begin();
  	while(it != lst.end()) {
		float newValue = *it;
		*it = *it - prevValue;
		prevValue = newValue;
		it++;
  	}
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Router::initFunction()
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
********************************************************************/
Model &Router::externalFunction( const ExternalMessage &msg )
{
	//[(!) update common variables]	
	this->sigma    = nextChange();	
	this->elapsed  = msg.time()-lastChange();	
 	this->timeLeft = this->sigma - this->elapsed; 

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
Model &Router::internalFunction( const InternalMessage &msg )
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
Model &Router::outputFunction( const CollectMessage &msg )
{
	// TODO cambiar para que escupa camiones
	sendOutput( msg.time(), out, 0) ;
	return *this;

}

Router::~Router()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
