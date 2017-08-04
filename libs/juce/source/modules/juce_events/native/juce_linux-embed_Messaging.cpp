/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2016 - ROLI Ltd.

   Permission is granted to use this software under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license/

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
   OF THIS SOFTWARE.

   -----------------------------------------------------------------------------

   To release a closed-source product which uses other parts of JUCE not
   licensed under the ISC terms, commercial licenses are available: visit
   www.juce.com for more information.

  ==============================================================================
*/

//==============================================================================
class EventsThread : public Thread
{
public:
    EventsThread()
        : Thread("EventsThread"),
          initializing(false) {}

    ~EventsThread()
    {
        signalThreadShouldExit();
        stopThread(2000);

        const ScopedLock sl(queueLock);
        jassert(queue.size() == 0);
        queue.clear();
    }

    bool postMessage(MessageManager::MessageBase* const msg)
    {
        const ScopedLock sl(queueLock);
        queue.add(msg);
        return true;
    }

    bool isInitializing() const noexcept
    {
        return initializing;
    }

protected:
    void run() override
    {
        /*
         * We need to know when we're initializing because MessageManager::setCurrentThreadAsMessageThread()
         * calls doPlatformSpecificInitialisation/Shutdown, in which we started this thread previously.
         * To avoid a deadlock we do not call start/stopThread if still initializing.
         */
        initializing = true;

        if (MessageManager* const msgMgr = MessageManager::getInstance())
            msgMgr->setCurrentThreadAsMessageThread();

        initializing = false;

        for (; ! threadShouldExit();)
        {
            // dispatch messages until no more present, then sleep
            for (; dispatchNextInternalMessage();) {}

            sleep(25);
        }
    }

private:
    volatile bool initializing;
    ReferenceCountedArray<MessageManager::MessageBase> queue;
    CriticalSection queueLock;

    MessageManager::MessageBase::Ptr popNextMessage()
    {
        const ScopedLock sl(queueLock);
        return queue.removeAndReturn(0);
    }

    bool dispatchNextInternalMessage()
    {
        if (const MessageManager::MessageBase::Ptr msg = popNextMessage())
        {
            JUCE_TRY
            {
                msg->messageCallback();
                return true;
            }
            JUCE_CATCH_EXCEPTION
        }

        return false;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EventsThread)
};

static EventsThread& getEventsThreadInstance()
{
    static EventsThread eventsThread;
    return eventsThread;
}

//==============================================================================
void MessageManager::doPlatformSpecificInitialisation()
{
    EventsThread& eventsThread(getEventsThreadInstance());

    if (! eventsThread.isInitializing())
        eventsThread.startThread();
}

void MessageManager::doPlatformSpecificShutdown()
{
    EventsThread& eventsThread(getEventsThreadInstance());

    if (! eventsThread.isInitializing())
        eventsThread.stopThread(-1);
}

bool MessageManager::postMessageToSystemQueue (MessageManager::MessageBase* const message)
{
    EventsThread& eventsThread(getEventsThreadInstance());
    return eventsThread.postMessage(message);
}

void MessageManager::broadcastMessage (const String& /* value */)
{
    /* TODO */
}

// this function expects that it will NEVER be called simultaneously for two concurrent threads
bool MessageManager::dispatchNextMessageOnSystemQueue (bool returnIfNoPendingMessages)
{
    DBG ("MessageManager::dispatchNextMessageOnSystemQueue() unsupported");
    return false;
}
