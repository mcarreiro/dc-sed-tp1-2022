#include "pmodeladm.h" 
#include "register.h"

#include "Barrier.h" 

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< Barrier >(), ATOMIC_MODEL_NAME); // Register custom models
}

