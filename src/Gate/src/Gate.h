#ifndef __Gate_H // cambiar nombre
#define __Gate_H // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <list>

#define ATOMIC_MODEL_NAME "Gate" // cambiar nombre

/** forward declarations **/
//TODO: add distribution class declaration here if needed
// Ej: class Distribution ;
class Distribution ;
//class Tuple<int>;

enum GateState {FREE, BUSY, UNAVAILABLE}

/** declarations **/
class Gate: public Atomic {
	public:
		Gate( const string &name = ATOMIC_MODEL_NAME ); // Default constructor
		~Gate(); // Destructor
		virtual string className() const {return ATOMIC_MODEL_NAME;}
	
	protected:
		Model &initFunction();	
		Model &externalFunction( const ExternalMessage & );
		Model &internalFunction( const InternalMessage & );
		Model &outputFunction( const CollectMessage & );
	
	private:
		int startHour; // hora del dia donde comienzan activo
		int endHour; // hora del dia donde termina activo

		VTime proccesingTimePacketA();
		VTime proccesingTimePacketB();
		VTime proccesingTimePacketC();
		VTime proccesingTimeTruck(VTime now, Tuple<Real> truck);
		int workersNow(VTime now);

		bool isActivePeriod(VTime now);
		VTime wakeUpSigmaAt(VTime now, int hourOfDay);
				
		Distribution *dist ;
		Distribution &distribution()	{ return *dist; }

		GateState currentState;
		Tuple<Real> currentTruck;

		const Port &fromBarrier;
		Port &toBarrier ;
		Port &out ;

		// Lifetime programmed since the last state transition to the next planned internal transition.
		VTime sigma;
		// Time elapsed since the last state transition until now
		VTime elapsed;
		// Time remaining to complete the last programmed Lifetime
		VTime timeLeft;	
};	// class Gate


#endif   //__Gate_H 
