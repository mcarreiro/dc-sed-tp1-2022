#include "pmodeladm.h" 
#include "register.h"

#include "Barrier.h" 
#include "Manager.h" 
#include "Router.h" 
#include "Gate.h" 

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< Barrier >(), BARRIER_ATOMIC_MODEL_NAME); // Register custom models
	admin.registerAtomic(NewAtomicFunction< Manager >(), MANAGER_ATOMIC_MODEL_NAME); // Register custom models
	admin.registerAtomic(NewAtomicFunction< Router >(), ROUTER_ATOMIC_MODEL_NAME); // Register custom models
	admin.registerAtomic(NewAtomicFunction< Gate >(), GATE_ATOMIC_MODEL_NAME); // Register custom models
}

