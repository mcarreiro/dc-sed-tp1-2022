#ifndef __Router_H // cambiar nombre
#define __Router_H // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <list>

#define ATOMIC_MODEL_NAME "Barrier" 


/** declarations **/
class Barrier: public Atomic {
	public:
		Barrier( const string &name = ATOMIC_MODEL_NAME ); // Default constructor
		~Barrier(); // Destructor
		virtual string className() const {return ATOMIC_MODEL_NAME;}
	
	protected:
		Model &initFunction();	
		Model &externalFunction( const ExternalMessage & );
		Model &internalFunction( const InternalMessage & );
		Model &outputFunction( const CollectMessage & );
	
	private:
		int amountOfGates; // cantidad de gates totales
		
		std::bitset<10> gates;

		int assignGate();

		// const Port &in;
		Port &out1 ;
		Port &out2 ;
		Port &out3 ;
		Port &out4 ;
		Port &out5 ;
		Port &out6 ;
		Port &out7 ;
		Port &out8 ;
		Port &out9 ;
		Port &out10 ;

		const Port &in;
		const Port &done;

		// Lifetime programmed since the last state transition to the next planned internal transition.
		VTime sigma;
		// Time elapsed since the last state transition until now
		VTime elapsed;
		// Time remaining to complete the last programmed Lifetime
		VTime timeLeft;	
};	// class Router


#endif   //__Router_H 
