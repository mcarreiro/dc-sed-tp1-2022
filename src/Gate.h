#ifndef __Gate_H // cambiar nombre
#define __Gate_H // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <list>

#define GATE_ATOMIC_MODEL_NAME "Gate" // cambiar nombre

/** forward declarations **/
//TODO: add distribution class declaration here if needed
// Ej: class Distribution ;
class Distribution ;
//class Tuple<int>;

enum GateState {FREE, BUSY, UNAVAILABLE};
enum BarrierUpdate {OPEN, CLOSED, NO_MESSAGE};

/** declarations **/


class Gate: public Atomic {
	public:
		Gate( const string &name = GATE_ATOMIC_MODEL_NAME ); // Default constructor
		~Gate(); // Destructor
		virtual string className() const {return GATE_ATOMIC_MODEL_NAME;}
	
	protected:
		Model &initFunction();	
		Model &externalFunction( const ExternalMessage & );
		Model &internalFunction( const InternalMessage & );
		Model &outputFunction( const CollectMessage & );
	
	private:
		int baseStartHour; //  hora del dia donde comienzan activo
		int baseEndHour; //  hora del dia donde termina activo
		int startHour; // hora del dia donde comienzan activo actual
		int endHour; // hora del dia donde termina activo actual
		void refreshActivePeriod(VTime now);
		void resetActivePeriod();
		void onManagerWakeUp(VTime now);

		VTime lastManagerWakeUp;

		VTime proccesingTimePacketA();
		VTime proccesingTimePacketB();
		VTime proccesingTimePacketC();
		VTime proccesingTimePacket(int type);

		VTime proccesingTimeTruck(VTime now, Tuple<Real> truck);
		int workersNow(VTime now);

		float workersBoost;
		VTime lastBoostUpdate;
		void refreshBoost(VTime now);
		void boost(VTime now);
		int getWorkersBoost();

		int gateId;
		int startWorkers;
		int workersAddPerBoost = 3;



		bool isActivePeriod(VTime now);
		bool isBaseActivePeriod(VTime now);

		VTime wakeUpAtSigma(VTime now, int hourOfDay);
				
		Distribution *dist ;
		Distribution &distribution()	{ return *dist; }

		GateState currentState;
		Tuple<Real> currentTruck;

		BarrierUpdate messageForBarrier; // proximo mensaje a enviar a la barrera

		const Port &fromBarrier;
		Port &toBarrier ;
		Port &out ;

		const Port &fromManager;

		// Lifetime programmed since the last state transition to the next planned internal transition.
		VTime sigma;
		// Time elapsed since the last state transition until now
		VTime elapsed;
		// Time remaining to complete the last programmed Lifetime
		VTime timeLeft;	
};	// class Gate


#endif   //__Gate_H 
