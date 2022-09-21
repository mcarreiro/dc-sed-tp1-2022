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
	in(addInputPort( "in" ))
{
	startHour = 7;
	endHour = 11;
}


Router::scheudleTrucksForTheDay() {
	// TODO cambiar estas tres a parametros ?
	int n = 200; 
	float mean = (endHour+startHour)/2.0;
	float stdev = 1.0;

	std::normal_distribution<float> normal(mean, stddev);

	while (scheudledTrucks.size() < n) {
		float sample = normal(gen);
		if (startHour <= sample and sample <= endHour ) {
			scheudledTrucks.push_back(sample);
		}
	}
	scheudledTrucks.sort();

	// iteramos la lista para obtener la diferencia con el anterior
	float prevValue = startHour;
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
		this->sigma = VTime(0,0,8,0); // TODO arreglar esto para que se levante siempre a la misma hora
	}
	else {
		float sigmaSeconds = scheudledTrucks.pop_front();
		this->sigma = VTime(0,0,sigmaSeconds,0);
	}

	holdIn( AtomicState::passive, this->sigma );
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
