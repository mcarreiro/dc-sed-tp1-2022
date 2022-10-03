#ifndef __Barrier_H // cambiar nombre
#define __Barrier_H // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <list>
#include <string>
#include <bitset>
using namespace std;

#define BARRIER_ATOMIC_MODEL_NAME "Barrier" 


/** declarations **/
class Barrier: public Atomic {
	public:
		Barrier( const string &name = BARRIER_ATOMIC_MODEL_NAME ); // Default constructor
		virtual string className() const {return BARRIER_ATOMIC_MODEL_NAME;}
	
	protected:
		Model &initFunction();	
		Model &externalFunction( const ExternalMessage & );
		Model &internalFunction( const InternalMessage & );
		Model &outputFunction( const CollectMessage & );
	
	private:
		int amountOfGates; // cantidad de gates totales
		int gateAssigned; // cantidad de gates totales
		Tuple<Real> next;
		Tuple<Real> empty;
		VTime preparationTime;
		bitset<10> gates; // fixed gates

		int assignGate();
		void externalFunctionHelper(const ExternalMessage &msg, int i);

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
		Port &done ;

		const Port &in;
		const Port &done1;
		const Port &done2;
		const Port &done3;
		const Port &done4;
		const Port &done5;
		const Port &done6;
		const Port &done7;
		const Port &done8;
		const Port &done9;
		const Port &done10;
};	// class Barrier


#endif   //__Barrier_H 
