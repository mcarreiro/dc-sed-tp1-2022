#include "pmodeladm.h" 
#include "register.h"

#include "Router.h" 

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< Router >(), ATOMIC_MODEL_NAME); // Register custom models
}

