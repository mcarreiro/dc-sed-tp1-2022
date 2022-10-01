#include "pmodeladm.h" 
#include "register.h"

#include "Gate.h" 

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< Gate >(), ATOMIC_MODEL_NAME); // Register custom models
}

