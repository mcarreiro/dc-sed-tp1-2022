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
#include <queue>
#include <algorithm> 
#include <vector>
#include <functional>
#include <math.h> 
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
	toBarrier(addOutputPort( "toBarrier" )),
	fromBarrier(addInputPort( "fromBarrier" )),
	fromManager(addInputPort( "fromManager" ))

{
	baseStartHour = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "startHour" ) );
	baseEndHour = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "endHour" ) );
	gateId = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "id" ) );

	startHour = baseStartHour;
	endHour = baseEndHour;

	lastBoostUpdate = VTime::Zero;
	workersBoost = 0.0;

	dist = Distribution::create("normal");
	MASSERT( dist ) ;
	dist->setVar( 0,0 ) ; // mean
	dist->setVar( 1,1 ) ; // var
	currentState = UNAVAILABLE;
 	currentTruck = Tuple<Real>();
	messageForBarrier = OPEN; // se asume que la barrier toma las gates como cerradas al principio

	lastManagerWakeUp = VTime::Zero;
}

void Gate::refreshBoost(VTime now) {
	VTime boostDuration = VTime(2,0,0,0); // duracion del boost
	float secondsSinceUpdate = (now - lastBoostUpdate).asSecs();

	float alpha = log2(workersAddPerBoost + 1.0);
	float decay = alpha*secondsSinceUpdate/((boostDuration.asSecs()));
	workersBoost = workersBoost * pow(2.0, -decay);

	lastBoostUpdate = now;
}
void Gate::boost(VTime now) {
	refreshBoost(now);
	workersBoost = workersAddPerBoost+1.0;
}

int Gate::getWorkersBoost() {
	int res = (int) workersBoost;
	return res;
}

bool Gate::isActivePeriod(VTime now) {
	int currentHours =(int) (now.asSecs()/3600);
	currentHours = currentHours % 24;
	return startHour <= currentHours && currentHours < endHour;
}

bool Gate::isBaseActivePeriod(VTime now) {
	int currentHours =(int) (now.asSecs()/3600);
	currentHours = currentHours % 24;
	return baseStartHour <= currentHours && currentHours < baseEndHour;
}

int Gate::workersNow(VTime now) {
	int currentHours =(int) (now.asSecs()/3600);
	currentHours = currentHours % 24;

	int workers = 0;

	// 5 maniana
	// 5 tarde
	// 4 overlap


	if (currentHours >= 9 && currentHours < 12) {
		workers = 5;
	}
	else if (currentHours >= 12 && currentHours < 18) {
		workers = 4;
	}
	else if (currentHours >= 18 && currentHours < 21) {
		workers = 5;
	}

	// 2 workers si estoy fuera de horario ?
	if (currentHours >= endHour || currentHours < startHour) {
		workers = 2;
	}	
	return workers + getWorkersBoost();
}


VTime Gate::proccesingTimeTruck(VTime now, Tuple<Real> truck) {
	refreshBoost(now);
	int workers = workersNow(now);

	int nA = (int) stof(truck[3].asString());
	int nB = (int) stof(truck[4].asString());
	int nC = (int) stof(truck[5].asString());

	vector<int> packetsToProccess;
	packetsToProccess.insert(packetsToProccess.end(), nA, 1);
	packetsToProccess.insert(packetsToProccess.end(), nB, 2);
	packetsToProccess.insert(packetsToProccess.end(), nC, 3);

	VTime res = VTime::Zero;
	priority_queue <VTime, vector<VTime>, greater<VTime>> processing;
	
	for (int i = 0; i < min(workers, nA+nB+nC); i++) {
		int packet = packetsToProccess.back();
		packetsToProccess.pop_back();
		processing.push(proccesingTimePacket(packet));
	}

	while (! processing.empty()) {

		// sacamos el proximo y restamos del remaining time del resto 
		priority_queue <VTime, vector<VTime>, greater<VTime>> temp;
		VTime next = processing.top();
		processing.pop();

		while (! processing.empty()) {
			temp.push(processing.top() - next);
			processing.pop();
		}
		processing = temp;

		// agregamos al tiempo total
		res = res + next;

		// si quedan paquetes por procesar agregamos uno mas por el worker desocupado
		if (packetsToProccess.size() > 0) {
			int packet = packetsToProccess.back();
			packetsToProccess.pop_back();
			processing.push(proccesingTimePacket(packet));
		}

	}

	return res;


}


VTime Gate::wakeUpAtSigma(VTime now, int hourOfDay) {
	int current_hours =(int) (now.asSecs()/3600);
	int current_hours_mod = current_hours%24;
	int hoursToNextAwake = 0;

	// es hoy
	if (hourOfDay > current_hours_mod ) {
		hoursToNextAwake = hourOfDay - current_hours_mod;
	}
	// es maniana
	else {
		hoursToNextAwake = (24 - current_hours_mod) + hourOfDay;
	}

	VTime targetAwake = VTime(hoursToNextAwake+current_hours,0,0,0) - now;
	return targetAwake;
}


VTime Gate::proccesingTimePacketA() {
	float mean = 20;
	float std = 10; // mas menos 10 segundos

	double seconds = 0;
	while (seconds <= 1) {
		double sample = distribution().get() * std + mean;
		seconds = sample;
	}
	return VTime(seconds);

}

VTime Gate::proccesingTimePacketB() {
	float mean = 20;
	float std = 10; 

	double seconds = 0;
	while (seconds <= 1) {
		double sample = distribution().get() * std + mean;
		seconds = sample;
	}
	return VTime(seconds);
}

VTime Gate::proccesingTimePacketC() {
	float mean = 20; 
	float std = 60; 

	double seconds = 0;
	while (seconds <= 1) {
		double sample = distribution().get() * std + mean;
		seconds = sample;
	}
	return VTime(seconds);
}

VTime Gate::proccesingTimePacket(int type) {
	switch (type) {
		case 0:
			return  proccesingTimePacketA();
			break;
		case 1:
			return proccesingTimePacketB();
			break;
		case 2:
			return proccesingTimePacketC();
			break;
	}

	return VTime::Zero;

}

void Gate::onManagerWakeUp(VTime now) {
	int current_hours = (int) (now.asSecs()/3600);
	current_hours = current_hours%24;
	startHour = current_hours;
	lastManagerWakeUp = now;

	// abierto por 2 horas?
	// endHour = min(startHour + 2, 23); 
	// TODO chequear el caso en el que lupea: Ej 23 + 2 -> 01
	// notar que dependiendo en que momento me llamen puede ser entre 1 y 2 horas abierto
	
	// o seguimos abiertos hasta mi horario de cierre habitual?
	endHour = baseEndHour; // esta linea es innecesaria pero la agrego por claridad

}


void Gate::resetActivePeriod() {
	startHour = baseStartHour;
	endHour = baseEndHour;
	lastManagerWakeUp = VTime::Zero;
}


// si estoy en mi periodo de actividad normal, reconfiguro el actual al normal
void Gate::refreshActivePeriod(VTime now) {
	int currentHours =(int) (now.asSecs()/3600);
	currentHours = currentHours%24;
	bool inNextDay = (lastManagerWakeUp > VTime::Zero) && (VTime(24,0,0,0) >= (now - lastManagerWakeUp)) ;

	if (inNextDay || (baseStartHour <= currentHours && currentHours < baseEndHour)) {
		resetActivePeriod();
	}
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Gate::initFunction()
{
	// [(!) Initialize common variables]
	this->elapsed  = VTime::Zero;
	this->sigma    = VTime(startHour,0,0,0);
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
	// asumo que barrier no me manda mensajes si estoy closed
	if (msg.port() == fromBarrier) {
		Tuple<Real> truck = Tuple<Real>::from_value(msg.value());
		currentTruck = truck;
		// route_id
		// arrival_date
		// assigned_date
		// exit_date
		// count_A
		// count_B
		// count_C
		// id_gate
		// boost_workers


		VTime proccesingTime = proccesingTimeTruck(msg.time(), truck);	

		Tuple<Real> out_value{truck[0], truck[1], truck[2], (msg.time() + proccesingTime).asSecs(), truck[4],truck[5], truck[6], gateId, getWorkersBoost()};
		currentTruck = out_value;
		
		this->sigma    = proccesingTime;
		this->elapsed  = VTime::Zero;
		this->timeLeft = this->sigma - this->elapsed; 

		currentState = BUSY;
		if (isActivePeriod(msg.time()+proccesingTime)) {
			messageForBarrier = OPEN;
		}
		else {
			messageForBarrier = NO_MESSAGE;
		}
	}
	else if (msg.port() == fromManager) {
		if (currentState == FREE) {
			currentTruck = Tuple<Real>();
			messageForBarrier = CLOSED;
			boost(msg.time());
			this->sigma    = nextChange();	
			this->elapsed  = msg.time()-lastChange();	
			this->timeLeft = this->sigma - this->elapsed; 		}
		else if (currentState == BUSY) {

			refreshActivePeriod(msg.time()+nextChange());
			boost(msg.time());

			if (isActivePeriod(msg.time()+nextChange())) {
				messageForBarrier = OPEN;
			}
			else {
				messageForBarrier = NO_MESSAGE;
			}

			this->sigma    = nextChange();	
			this->elapsed  = msg.time()-lastChange();	
			this->timeLeft = this->sigma - this->elapsed; 
		}
		else if (currentState == UNAVAILABLE) {
			onManagerWakeUp(msg.time());

			this->sigma = VTime::Zero;
			this->elapsed  = VTime::Zero;
			this->timeLeft = this->sigma - this->elapsed; 
			messageForBarrier = OPEN;
		}
	}

	holdIn( AtomicState::active, this->sigma  ) ;

	
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &Gate::internalFunction( const InternalMessage &msg )
{
	refreshActivePeriod(msg.time());

	// UNAVAILABLE -> FREE (empezo el dia)
	// BUSY -> FREE (termino truck)
	// BUSY -> UNAVAILABLE (termino truck y se acabo el dia)
	// FREE -> UNAVAILABLE (se acabo el dia)

	if (currentState == UNAVAILABLE) {
		currentTruck = Tuple<Real>();
		currentState = FREE;
		this->sigma = wakeUpAtSigma(msg.time(),endHour );
		
		if (isBaseActivePeriod(msg.time() + this->sigma)) {
			messageForBarrier = NO_MESSAGE;
		}
		else {
			messageForBarrier = CLOSED;
		}

	}

	else if (currentState == BUSY) {
		currentTruck = Tuple<Real>();
		if (isActivePeriod(msg.time())) {
			currentState = FREE;
			this->sigma = wakeUpAtSigma(msg.time(),endHour );
			messageForBarrier = CLOSED;
		}
		else {
			currentState = UNAVAILABLE;
			resetActivePeriod();
			this->sigma = wakeUpAtSigma(msg.time(),startHour );
			messageForBarrier = OPEN;
		}
	}

	else if (currentState == FREE ) {
		resetActivePeriod();
		currentTruck = Tuple<Real>();

		if (isActivePeriod(msg.time())) {
			this->sigma =  wakeUpAtSigma(msg.time(),endHour );
			messageForBarrier = CLOSED;
		}
		else{
			currentState = UNAVAILABLE;
			this->sigma =  wakeUpAtSigma(msg.time(),startHour );
			messageForBarrier = OPEN;
		}
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

	if (messageForBarrier == OPEN) {
		int send = 0;
		sendOutput( msg.time(), toBarrier, send) ;

		
	}
	else if (messageForBarrier == CLOSED) {
		int send = 1;
		sendOutput( msg.time(), toBarrier, send) ;

	}


	// si termino un camion lo outputeo
	if ( (currentState == BUSY) and currentTruck.size() != 0) {
		sendOutput( msg.time(), out, currentTruck) ;
	}
	return *this;

}

Gate::~Gate()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
