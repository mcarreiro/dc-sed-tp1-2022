#ifndef __Manager_H // cambiar nombre
#define __Manager_H // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <list>
#include <string>
#include <bitset>

#define MANAGER_ATOMIC_MODEL_NAME "Manager" 


/** declarations **/
class Manager: public Atomic {
	public:
		Manager( const string &name = MANAGER_ATOMIC_MODEL_NAME ); // Default constructor
		virtual string className() const {return MANAGER_ATOMIC_MODEL_NAME;}
	
	protected:
		Model &initFunction();	
		Model &externalFunction( const ExternalMessage & );
		Model &internalFunction( const InternalMessage & );
		Model &outputFunction( const CollectMessage & );
	
	private:
		int amountOfGates; // cantidad de gates totales
		int function; //0 is buff, 1 is open gates
		VTime preparationTime;
		int threshold;
		bool isEmergencyData;
		std::bitset<10> gates; // fixed gates

		int act();
		bool isEmergency(Real throughput);

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
};	// class Manager

#endif   //__Manager_H 
