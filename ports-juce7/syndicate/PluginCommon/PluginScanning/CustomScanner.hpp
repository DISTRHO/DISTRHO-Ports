#pragma once

#include <JuceHeader.h>
#include "AllUtils.h"

class Superprocess : private juce::ChildProcessCoordinator {
public:
    Superprocess(juce::AudioProcessor::WrapperType wrapperType) {
        juce::Logger::writeToLog("Launching scan");
        launchWorkerProcess(Utils::getPluginScanServerBinary(wrapperType), Utils::PLUGIN_SCAN_SERVER_UID, 0, 0);
    }

    enum class State {
        timeout,
        gotResult,
        connectionLost,
    };

    struct Response {
        State state;
        std::unique_ptr<juce::XmlElement> xml;
    };

    Response getResponse() {
        std::unique_lock<std::mutex> lock { mutex };

        if (! condvar.wait_for (lock, std::chrono::milliseconds { 50 }, [&] { return gotResult || connectionLost; })) {
            return { State::timeout, nullptr };
        }

        const auto state = connectionLost ? State::connectionLost : State::gotResult;
        connectionLost = false;
        gotResult = false;

        return {state, std::move (pluginDescription)};
    }

    using ChildProcessCoordinator::sendMessageToWorker;

private:
    void handleMessageFromWorker(const juce::MemoryBlock& mb) override {
        const std::lock_guard<std::mutex> lock { mutex };
        pluginDescription = parseXML (mb.toString());
        gotResult = true;
        condvar.notify_one();
    }

    void handleConnectionLost() override {
        juce::Logger::writeToLog("Connection lost");
        const std::lock_guard<std::mutex> lock { mutex };
        connectionLost = true;
        condvar.notify_one();
    }

    std::mutex mutex;
    std::condition_variable condvar;

    std::unique_ptr<juce::XmlElement> pluginDescription;
    bool connectionLost = false;
    bool gotResult = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Superprocess)
};

class CustomPluginScanner : public juce::KnownPluginList::CustomScanner {
public:
    CustomPluginScanner(juce::AudioProcessor::WrapperType wrapperType)
        : _wrapperType(wrapperType) { }

    ~CustomPluginScanner() override { }

    bool findPluginTypesFor(juce::AudioPluginFormat& format,
                            juce::OwnedArray<juce::PluginDescription>& result,
                             const juce::String& fileOrIdentifier) override {
        if (addPluginDescriptions(format.getName(), fileOrIdentifier, result)) {
            return true;
        }

        superprocess = nullptr;
        return false;
    }

    void scanFinished() override {
        superprocess = nullptr;
    }

private:
    /*  Scans for a plugin with format 'formatName' and ID 'fileOrIdentifier' using a subprocess,
        and adds discovered plugin descriptions to 'result'.

        Returns true on success.

        Failure indicates that the subprocess is unrecoverable and should be terminated.
    */
    bool addPluginDescriptions(const juce::String& formatName,
                               const juce::String& fileOrIdentifier,
                               juce::OwnedArray<juce::PluginDescription>& result) {
        if (superprocess == nullptr) {
            superprocess = std::make_unique<Superprocess>(_wrapperType);
        }

        juce::MemoryBlock block;
        juce::MemoryOutputStream stream { block, true };
        stream.writeString(formatName);
        stream.writeString(fileOrIdentifier);

        if (!superprocess->sendMessageToWorker(block)) {
            return false;
        }

        for (;;) {
            if (shouldExit()) {
                return true;
            }

            const auto response = superprocess->getResponse();

            if (response.state == Superprocess::State::timeout) {
                continue;
            }

            if (response.xml != nullptr) {
                for (const auto* item : response.xml->getChildIterator()) {
                    auto desc = std::make_unique<juce::PluginDescription>();

                    if (desc->loadFromXml (*item)) {
                        result.add (std::move (desc));
                    }
                }
            }

            return (response.state == Superprocess::State::gotResult);
        }
    }

    const juce::AudioProcessor::WrapperType _wrapperType;
    std::unique_ptr<Superprocess> superprocess;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomPluginScanner)
};
