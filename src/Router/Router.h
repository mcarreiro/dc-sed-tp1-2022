#ifndef __Router_H // cambiar nombre
#define __Router_H // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <list>

#define ATOMIC_MODEL_NAME "Router" // cambiar nombre

/** forward declarations **/
//TODO: add distribution class declaration here if needed
// Ej: class Distribution ;

/** declarations **/
class Router: public Atomic {
	public:
		Router( const string &name = ATOMIC_MODEL_NAME ); // Default constructor
		~Router(); // Destructor
		virtual string className() const {return ATOMIC_MODEL_NAME;}
	
	protected:
		Model &initFunction();	
		Model &externalFunction( const ExternalMessage & );
		Model &internalFunction( const InternalMessage & );
		Model &outputFunction( const CollectMessage & );
	
	private:
		int startHour; // hora del dia donde comienzan a llegar camiones
		int endHour; // hora del dia donde dejan de llegar camiones

		void scheudleTrucksForTheDay();

		// va ser una lista de sigmas indicando la espera al siguiente
		list<float> scheudledTrucks;

		
		// para usar normales de c++. Probablemente se cambie para usar normales de cd++
    	std::random_device rd; 
		std::mt19937 gen(rd()); 


		const Port &in;
		Port &out ;
		// Lifetime programmed since the last state transition to the next planned internal transition.
		VTime sigma;
		// Time elapsed since the last state transition until now
		VTime elapsed;
		// Time remaining to complete the last programmed Lifetime
		VTime timeLeft;	
};	// class Router


#endif   //__Router_H 
