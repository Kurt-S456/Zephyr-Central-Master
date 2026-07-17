#include "benchmark_shared.h"

int main(void)
{
#if defined(CONFIG_ROLE_WORKER)
	return run_worker();
#elif defined(CONFIG_ROLE_CHILD)
	return run_child();
#else
#error "Either CONFIG_ROLE_WORKER or CONFIG_ROLE_CHILD must be selected"
#endif
}
