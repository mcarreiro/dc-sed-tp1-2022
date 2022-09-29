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
		" - timeleft: " << timeLeft <<\
		" - elapsed: " << elapsed <<\
		" - sigma: " << sigma << endl;\
}


/** public functions **/

/*******************************************************************
* Function Name: [#MODEL_NAME#]
* Description: constructor
********************************************************************/
Router::Router( const string &name ) : 
	Atomic( name ),
	out(addOutputPort( "out" ))
{
	startHour = 7;
	endHour = 23;
	dist = Distribution::create("normal");
	MASSERT( dist ) ;
	dist->setVar( 0,0 ) ; // mean
	dist->setVar( 1,1 ) ; // var

	distPacketA = Distribution::create("exponential");
	MASSERT( distPacketA ) ;
	distPacketA->setVar( 0,5 ) ; // mean

	distPacketB = Distribution::create("exponential");
	MASSERT( distPacketB ) ;
	distPacketB->setVar( 0,5 ) ; // mean

	distPacketC = Distribution::create("exponential");
	MASSERT( distPacketC ) ;
	distPacketC->setVar( 0,5 ) ; // mean

	scheudleTrucksForTheDay();
}


void Router::scheudleTrucksForTheDay() {
	// TODO cambiar estas tres a parametros ?
	int n = 5; 
	float mean = (endHour+startHour)/2.0;
	float stdev = 2.0;

	while (scheudledTrucks.size() < n) {
		double sample = distribution().get();
		sample = sample*stdev + mean;
		
		if (startHour <= sample and sample <= endHour ) {
			sample = sample * 3600;
			scheudledTrucks.push_back(VTime( sample ));
		}
	}

	scheudledTrucks.sort();

	// iteramos la lista para obtener la diferencia con el anterior
	VTime prevValue = VTime(startHour,0,0,0);
	auto  it = scheudledTrucks.begin();
  	while(it != scheudledTrucks.end()) {
		VTime newValue = *it;
		*it = *it - prevValue;
		prevValue = newValue;
		it++;
  	}
}


int Router::getPacketsInTruck(float probability) {
	int res = (int) (*distPacketC).get();
	return res;

}


/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Router::initFunction()
{
	// [(!) Initialize common variables]
	this->elapsed  = VTime::Zero;
 	this->timeLeft = VTime(startHour,0,0,0);
 	// this->sigma = VTime::Inf; // stays in active state until an external event occurs;
 	this->sigma    = VTime(startHour,0,0,0); // force an internal transition in t=0;

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


	if (scheudledTrucks.size() == 0) {
		scheudleTrucksForTheDay();
		int current_hours =(int) (msg.time().asSecs()/3600);
		int hoursToNextAwake = (24 - (current_hours)%24) + startHour;
		float inSeconds = (hoursToNextAwake)*3600;
		
		VTime targetAwake = VTime(hoursToNextAwake+current_hours,0,0,0) - msg.time()  + scheudledTrucks.front();
		scheudledTrucks.pop_front();
		scheudledTrucks.push_front(targetAwake) ;

		cout << "hoursToNextAwake " << hoursToNextAwake << endl;
		cout << "end day wait " << targetAwake.asString() << endl;
		cout << "wake up at " <<  VTime(hoursToNextAwake+current_hours,0,0,0).asString() << endl;
		cout << "now " <<  msg.time().asString() << endl;
		//this->sigma = VTime(8,0,0,0);
	}

	this->sigma = scheudledTrucks.front();
	scheudledTrucks.pop_front();

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
Model &Router::outputFunction( const CollectMessage &msg )
{
	// TODO cambiar para que escupa camiones
	int hola = getPacketsInTruck(4.0);
	//Tuple<int> t = Tuple<int>(0,0,0);
	cout << "envio " << hola << endl;
	sendOutput( msg.time(), out, hola) ;
	return *this;

}

Router::~Router()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
