/*
 *  File:       AudioSpinMutex.h
 *
 *  Created:    22/10/2021
 *
 *	This file is part of WECore.
 *
 *  WECore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WECore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WECore.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <array>
#include <thread>
#include <atomic>

// Some useful links about these instructions in notes/splnlock-instructions.txt
#if defined(__x86_64__) || defined(_M_AMD64)
    #include <emmintrin.h>
    #define CPU_PAUSE _mm_pause();
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define CPU_PAUSE __asm__ __volatile__("yield" ::: "memory");
#else
    #error Unsupported architecture
#endif

namespace WECore {

    /**
     * Provides mutex that the audio thread can try-lock in a realtime safe way.
     *
     * Based on the implementation described here:
     * https://timur.audio/using-locks-in-real-time-audio-processing-safely
     */
    class AudioSpinMutex {
    public:
        AudioSpinMutex() = default;
        ~AudioSpinMutex() = default;

        /**
         * Call from the non-realtime thread.
         *
         * Will block until the mutex is locked.
         */
        void lock() {
            constexpr std::array iterations = {5, 10, 3000};

            for (int i = 0; i < iterations[0]; ++i) {
                if (tryLock()) {
                    return;
                }
            }

            for (int i = 0; i < iterations[1]; ++i) {
                if (tryLock()) {
                    return;
                }

                CPU_PAUSE
            }

            while (true) {
                for (int i = 0; i < iterations[2]; ++i) {
                    if (tryLock()) {
                        return;
                    }

                    CPU_PAUSE
                    CPU_PAUSE
                    CPU_PAUSE
                    CPU_PAUSE
                    CPU_PAUSE
                    CPU_PAUSE
                    CPU_PAUSE
                    CPU_PAUSE
                    CPU_PAUSE
                    CPU_PAUSE
                }

                // Waiting longer than we should, let's give other threads
                // a chance to recover
                std::this_thread::yield();
            }
        }

        /**
         * Returns true if the lock was successful.
         *
         * Is safe to call from the audio thread.
         */
        bool tryLock() {
            return !flag.test_and_set(std::memory_order_acquire);
        }

        /**
         * Call to unlock the mutex.
         */
        void unlock() {
            flag.clear(std::memory_order_release);
        }

    private:
        std::atomic_flag flag = ATOMIC_FLAG_INIT;
    };

    class AudioSpinLockBase {
    public:
        AudioSpinLockBase(AudioSpinMutex& mutex) : _mutex(mutex), _isLocked(false) { }

        ~AudioSpinLockBase() {
            if (_isLocked) {
                _mutex.unlock();
            }
        }

        void unlock() {
            if (_isLocked) {
                _mutex.unlock();
                _isLocked = false;
            }
        }

        bool isLocked() { return _isLocked; }

    protected:
        AudioSpinMutex& _mutex;

        // Keeps track of whether this lock is still holding the mutex. Must always check this
        // internally before calling unlock on the mutex, otherwise we might unlock it, then the lock
        // is taken by someone else, then we unlock it again.
        bool _isLocked;
    };

    /**
     * Locks the given AudioSpinMutex on constuction, unlocks it on destruction.
     */
    class AudioSpinLock : public AudioSpinLockBase {
    public:
        explicit AudioSpinLock(AudioSpinMutex& mutex) : AudioSpinLockBase(mutex) {
            _mutex.lock();
            _isLocked = true;
        }
    };

    /**
     * Will try lock the given AudioSpinMutex on construction and unlock on desctruction if needed.
     */
    class AudioSpinTryLock : public AudioSpinLockBase {
    public:
        explicit AudioSpinTryLock(AudioSpinMutex& mutex) : AudioSpinLockBase(mutex) {
            _isLocked = _mutex.tryLock();
        }
    };
}
