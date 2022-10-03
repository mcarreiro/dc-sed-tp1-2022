#ifndef __Router_H // cambiar nombre
#define __Router_H // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <list>

#define ROUTER_ATOMIC_MODEL_NAME "Router" // cambiar nombre

/** forward declarations **/
//TODO: add distribution class declaration here if needed
// Ej: class Distribution ;
class Distribution ;
//class Tuple<int>;

/** declarations **/
class Router: public Atomic {
	public:
		Router( const string &name = ROUTER_ATOMIC_MODEL_NAME ); // Default constructor
		~Router(); // Destructor
		virtual string className() const {return ROUTER_ATOMIC_MODEL_NAME;}
	
	protected:
		Model &initFunction();	
		Model &externalFunction( const ExternalMessage & );
		Model &internalFunction( const InternalMessage & );
		Model &outputFunction( const CollectMessage & );
	
	private:
		int startHour; // hora del dia donde comienzan a llegar camiones
		int endHour; // hora del dia donde dejan de llegar camiones
		int meanPacketsPerDay;
		
		Distribution *dist ;
		Distribution &distribution()	{ return *dist; }
		void scheudleTrucksForTheDay();
		Distribution *distPacketA;
		Distribution *distPacketB; 
		Distribution *distPacketC; 

		// va ser una lista de sigmas indicando la espera al siguiente
		list<VTime> scheudledTrucks;
		int id; // para incrementar con cada camion
		

		// const Port &in;
		// Port to output each truck
		Port &out ;
		// Port to output ID only
		Port &outId ;

		// Lifetime programmed since the last state transition to the next planned internal transition.
		VTime sigma;
		// Time elapsed since the last state transition until now
		VTime elapsed;
		// Time remaining to complete the last programmed Lifetime
		VTime timeLeft;	
};	// class Router


#endif   //__Router_H 
