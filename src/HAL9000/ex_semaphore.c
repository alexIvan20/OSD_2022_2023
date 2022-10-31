#include "HAL9000.h"
#include "synch.h"
#include "ex_semaphore.h"
#include "thread_internal.h"

void
SemaphoreInit(
    OUT     PSEMAPHORE      Semaphore,
    IN      DWORD           InitialValue
)
{
    ASSERT(NULL != Semaphore);
    memzero(Semaphore, sizeof(Semaphore));

    LockInit(&Semaphore->SemaphoreLock);

    Semaphore->Value = 0;
    if (InitialValue > 0) {
        Semaphore->Value = InitialValue;
    }
}

void
SemaphoreDown(
    INOUT   PSEMAPHORE      Semaphore,
    IN      DWORD           Value
)
{
    ASSERT(NULL != Semaphore);

    INTR_STATE state;
    LockAcquire(&Semaphore->SemaphoreLock, &state);

    while (Semaphore->Value - Value < 0) {
        LockRelease(&Semaphore->SemaphoreLock, state);
        ThreadYield();
        LockAcquire(&Semaphore->SemaphoreLock, &state);
    }

    Semaphore->Value -= Value;
    LockRelease(&Semaphore->SemaphoreLock, state);
}

void
SemaphoreUp(
    INOUT   PSEMAPHORE      Semaphore,
    IN      DWORD           Value
)
{
    ASSERT(NULL != Semaphore);

    INTR_STATE state;
    LockAcquire(&Semaphore->SemaphoreLock, &state);

    Semaphore->Value += Value;

    LockRelease(&Semaphore->SemaphoreLock, state);
}