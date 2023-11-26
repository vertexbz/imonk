#pragma once

#include <pico/sem.h>

namespace Job {
    class Semaphore {
    public:
        typedef int16_t Count;
    private:
#ifndef SOFT_SEM
        semaphore _sem{};
#else
        volatile Count _sem;
        Count _max;
#endif
    public:
        /*! \brief  Create a semaphore instance
         *
         * \param initial_permits How many permits are initially available to acquire
         * \param max_permits  Total number of permits allowed for this semaphore
         */
        Semaphore(Count initial_permits, Count max_permits);

        /*! \brief  Return number of available permits on the semaphore
         *
         * \return The number of permits available on the semaphore.
         */
        int available();

        /*! \brief  Release a permit on a semaphore
         *
         * Increases the number of permits by one (unless the number of permits is already at the maximum).
         * A blocked `acquire` will be released if the number of permits is increased.
         *
         * \return true if the number of permits available was increased.
         */
        bool release();


        /*! \brief  Acquire a permit from the semaphore
         *
         * This function will block and wait if no permits are available.
         */
        void acquire_blocking();
    };

}