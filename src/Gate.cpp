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
	startHour = baseStartHour;
	endHour = baseEndHour;

	dist = Distribution::create("normal");
	MASSERT( dist ) ;
	dist->setVar( 0,0 ) ; // mean
	dist->setVar( 1,1 ) ; // var
	currentState = UNAVAILABLE;
 	currentTruck = Tuple<Real>();
	messageForBarrier = NO_MESSAGE; // se asume que la barrier toma las gates como cerradas al principio

	lastManagerWakeUp = VTime::Zero;
}

bool Gate::isActivePeriod(VTime now) {
	int currentHours =(int) (now.asSecs()/3600);
	currentHours = currentHours % 24;
	return startHour <= currentHours && currentHours < endHour;
}

int Gate::workersNow(VTime now) {
	int currentHours =(int) (now.asSecs()/3600);
	currentHours = currentHours % 24;

	int workers = 0;

	// 2 workers turno maniana
	// 2 workers turno noche
	// cierto overlap turno maniana / turno noche


	if (currentHours >= 9 && currentHours < 18) {
		workers += 2;
	}
	if (currentHours >= 12 && currentHours < 21) {
		workers += 2;
	}

	// 2 workers si estoy fuera de horario ?
	if (currentHours >= endHour || currentHours < startHour) {
		workers = 2;
	}	
	return workers;
}


VTime Gate::proccesingTimeTruck(VTime now, Tuple<Real> truck) {
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

		this->sigma    = proccesingTimeTruck(msg.time(), truck);	
		this->elapsed  = VTime::Zero;
		this->timeLeft = this->sigma - this->elapsed; 

		Tuple<Real> out_value{truck[0], truck[1], truck[2], (msg.time() + this->sigma).asSecs(), truck[3],truck[4], truck[5]};
		currentTruck = out_value;
		currentState = BUSY;
		messageForBarrier = NO_MESSAGE;
	}
	else if (msg.port() == fromManager) {
		// ignoro al manager si no estoy cerrado
		if (currentState == FREE) {
			currentTruck = Tuple<Real>();
			messageForBarrier = NO_MESSAGE;
			this->sigma    = nextChange();	
			this->elapsed  = msg.time()-lastChange();	
			this->timeLeft = this->sigma - this->elapsed; 		}
		else if (currentState == BUSY) {
			messageForBarrier = NO_MESSAGE;
			this->sigma    = nextChange();	
			this->elapsed  = msg.time()-lastChange();	
			this->timeLeft = this->sigma - this->elapsed; 
		}
		else if (currentState == UNAVAILABLE) {
			currentState = FREE;
			messageForBarrier = OPEN;
			onManagerWakeUp(msg.time());
			this->sigma = wakeUpAtSigma(msg.time(),endHour );
			this->elapsed  = VTime::Zero;
			this->timeLeft = this->sigma - this->elapsed; 
		}
	}
	
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
	refreshActivePeriod(msg.time());

	// UNAVAILABLE -> FREE (empezo el dia)
	// BUSY -> FREE (termino truck)
	// BUSY -> UNAVAILABLE (termino truck y se acabo el dia)
	// FREE -> UNAVAILABLE (se acabo el dia)

	if (currentState == UNAVAILABLE) {
		currentTruck = Tuple<Real>();
		currentState = FREE;
		// cout << "me abro " << msg.time().asString() << endl;
		messageForBarrier = OPEN;
		this->sigma = wakeUpAtSigma(msg.time(),endHour );
	}

	else if (currentState == BUSY) {
		if (isActivePeriod(msg.time())) {
			currentState = FREE;
			messageForBarrier = OPEN;
			this->sigma = wakeUpAtSigma(msg.time(),endHour );
		}
		else {
			currentState = UNAVAILABLE;
			// no envio mensaje para barrier porque asumo que ya me tiene en close
			messageForBarrier = NO_MESSAGE;
			resetActivePeriod();
			this->sigma = wakeUpAtSigma(msg.time(),startHour );
		}
	}

	else if (currentState == FREE ) {
		resetActivePeriod();
		currentTruck = Tuple<Real>();
		// puede que tenga que seguir activo porque me activo el manager y ahora estoy activo normal
		if (isActivePeriod(msg.time())) {
			this->sigma =  wakeUpAtSigma(msg.time(),endHour );
			messageForBarrier = NO_MESSAGE;
		}
		else{
			// cout << "me cierro " << msg.time().asString() << endl;

			currentState = UNAVAILABLE;
			messageForBarrier = CLOSED;
			this->sigma =  wakeUpAtSigma(msg.time(),startHour );
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
	if ( (currentState == FREE or currentState == UNAVAILABLE) and currentTruck.size() != 0) {
		sendOutput( msg.time(), out, currentTruck) ;
	}
	return *this;

}

Gate::~Gate()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
