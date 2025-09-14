/*
 *	File:		CoreAudioProcessor.h
 *
 *	Version:	1.0.0
 *
 *	Created:	10/06/2017
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
 *
 */

#pragma once

#include "JuceHeader.h"
#include "General/ParameterDefinition.h"
#include "ParameterUpdateHandler.h"
#include "CustomParameter.h"

namespace WECore::JUCEPlugin {

    /**
     * This class provides basic functionality that is commonly used by an AudioProcessor in a
     * White Elephant plugin.
     *
     * Classes inheriting from this should:
     *   - Call registerParameter to declare parameters
     */
    class CoreAudioProcessor : public juce::AudioProcessor,
                               public ParameterUpdateHandler {
    public:
        inline CoreAudioProcessor();
        inline CoreAudioProcessor(const BusesProperties& ioLayouts);
        inline virtual ~CoreAudioProcessor();

        /**
         * Sets a given parameter using a value in the internal (non-normalised) range.
         */
        /** @{ */
        inline void setParameterValueInternal(juce::AudioParameterFloat* param, float value);
        inline void setParameterValueInternal(juce::AudioParameterInt* param, int value);
        inline void setParameterValueInternal(juce::AudioParameterBool* param, bool value);
        /** @} */

        /**
         * Adds a listener that will be notified whenever a parameter is changed.
         */
        void addParameterChangeListener(juce::ChangeListener* listener) {
            _parameterBroadcaster.addChangeListener(listener);
        }

        /**
         * Removes a previously added listener.
         */
        void removeParameterChangeListener(juce::ChangeListener* listener) {
            _parameterBroadcaster.removeChangeListener(listener);
        }

        /**
         * Calls writeToXml and stores the XML in the given memory block.
         */
        inline virtual void getStateInformation(juce::MemoryBlock& destData) override;

        /**
         * Collects the registered parameter values and writes them to XML.
         *
         * Float parameters are written in their normalised 0 to 1 range.
         *
         * Int parameters are written as a float representation of their real (not normalised) value.
         *
         * Bool parameters are written as a float representation of true or false.
         */
        inline std::unique_ptr<juce::XmlElement> writeToXml();

        /**
         * Reads the given memory into an XmlElement and calls restoreFromXml.
         */
        inline virtual void setStateInformation(const void* data, int sizeInBytes) override;

        /**
         * Restores parameter values from previously written XML.
         */
        inline void restoreFromXml(std::unique_ptr<juce::XmlElement> element);

    protected:
        /**
         * Used to register public parameters that are visible to the host.
         *
         * The parameter will be deleted when the processor class is deallocated.
         *
         * Float parameters are created with their real (not normalised) ranges.
         *
         * Int parameters are created with their real (not normalised) ranges.
         *
         * (Bool parameters don't have a meaningful range.)
         */
        /** @{ */
        inline void registerParameter(juce::AudioParameterFloat*& param,
                                      const juce::String& name,
                                      const ParameterDefinition::RangedParameter<double>* range,
                                      float defaultValue,
                                      float precision);

        inline void registerParameter(juce::AudioParameterFloat*& param,
                                      const juce::String& name,
                                      const ParameterDefinition::RangedParameter<float>* range,
                                      float defaultValue,
                                      float precision);

        inline void registerParameter(juce::AudioParameterInt*& param,
                                      const juce::String& name,
                                      const ParameterDefinition::BaseParameter<int>* range,
                                      int defaultValue);

        inline void registerParameter(juce::AudioParameterBool*& param,
                                      const juce::String& name,
                                      bool defaultValue);
        /** @} */

        /**
         * Used to register private parameters that are not visible to the host. Used for parameters
         * that cause other parameter or state changes so make automation impractical, but should
         * still be saved/restored and trigger updates through the ParameterBroadcaster as usual.
         *
         * These parameters typically need a custom setter when being restored to trigger the extra
         * parameter/state changes.
         *
         * The parameter will be deleted when the processor class is deallocated.
         *
         * Float parameters are created with their real (not normalised) ranges.
         *
         * Int parameters are created with their real (not normalised) ranges.
         *
         * Custom parameters don't have a range and can only ever be private.
         *
         * (Bool parameters don't have a meaningful range.)
         *
         */
        /** @{ */
        inline void registerPrivateParameter(juce::AudioParameterFloat*& param,
                                             const juce::String& name,
                                             const ParameterDefinition::RangedParameter<double>* range,
                                             float defaultValue,
                                             float precision,
                                             std::function<void(float)> setter);

        inline void registerPrivateParameter(juce::AudioParameterInt*& param,
                                             const juce::String& name,
                                             const ParameterDefinition::BaseParameter<int>* range,
                                             int defaultValue,
                                             std::function<void(int)> setter);

        template <typename PARAM_TYPE>
        inline void registerPrivateParameter(PARAM_TYPE*& param, const juce::String& name);
        /** @} */

        /**
         * Override this and return a vector of parameter names corresponding to the order that
         * parameters were stored in using the legacy schema.
         */
        virtual std::vector<juce::String> _provideParamNamesForMigration() = 0;

        /**
         * Override this to migrate saved parameter values from normalised to internal.
         */
        virtual void _migrateParamValues(std::vector<float>& paramValues) = 0;

    private:

        // Increment this after changing how parameter states are stored
        static constexpr int PARAMS_SCHEMA_VERSION {1};

        // We need to store pointers to all the private parameters that are registered so that they
        // can be deallocated by the destructor
        std::vector<juce::AudioProcessorParameter*> _privateParameters;
        std::vector<CustomParameter*> _customParameters;

        /**
         * Stores a setter and getter for a parameter. Used when persisting parameter values to XML
         * and restoring values from XML.
         */
        struct ParameterInterface {
            juce::String name;
            std::function<float()> getter;
            std::function<void(float)> setter;
        };

        /**
         * Stores a setter and getter for a custom parameter. Used when persisting parameter values
         * to XML and restoring values from XML. Access to the params XML element is provided since
         * custom parameters typically need to store complex data.
         */
        struct CustomParameterInterface {
            juce::String name;
            std::function<void(juce::XmlElement*)> writeToXml;
            std::function<void(juce::XmlElement*)> restoreFromXml;
        };

        /**
         * Listens for parameter changes and triggers the broadcaster so the changes can be handled
         * by another thread.
         */
        class ParameterBroadcaster : public juce::AudioProcessorParameter::Listener,
                                     public juce::ChangeBroadcaster {
        public:
            ParameterBroadcaster() = default;
            virtual ~ParameterBroadcaster() = default;

            virtual void parameterValueChanged(int /*parameterIndex*/, float /*newValue*/) override {
                this->sendChangeMessage();
            }

            virtual void parameterGestureChanged(int /*parameterIndex*/, bool /*gestureIsStarting*/) override {}
        };

        ParameterBroadcaster _parameterBroadcaster;

        /**
         * List of parameters which will trigger updates and are stored in XML.
         */
        std::vector<ParameterInterface> _paramsList;
        std::vector<CustomParameterInterface> _customParamsList;

        inline std::vector<float> _stringToFloatVector(const juce::String sFloatCSV) const;

        inline std::unique_ptr<juce::XmlElement> _migrateParameters(
                std::unique_ptr<juce::XmlElement> rootElement);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreAudioProcessor)
    };

    CoreAudioProcessor::CoreAudioProcessor() {
        addParameterChangeListener(&_parameterListener);
    }

    CoreAudioProcessor::CoreAudioProcessor(const BusesProperties& ioLayouts) : juce::AudioProcessor(ioLayouts) {
        addParameterChangeListener(&_parameterListener);
    }

    CoreAudioProcessor::~CoreAudioProcessor() {
        for (juce::AudioProcessorParameter* parameter : _privateParameters) {
            delete parameter;
        }

        for (CustomParameter* parameter : _customParameters) {
            delete parameter;
        }
    }

    void CoreAudioProcessor::setParameterValueInternal(juce::AudioParameterFloat* param, float value) {
        param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(value));
    }

    void CoreAudioProcessor::setParameterValueInternal(juce::AudioParameterInt* param, int value) {
        param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(value));
    }

    void CoreAudioProcessor::setParameterValueInternal(juce::AudioParameterBool* param, bool value) {
        param->setValueNotifyingHost(value);
    }

    void CoreAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
        std::unique_ptr<juce::XmlElement> element = writeToXml();
        copyXmlToBinary(*element.get(), destData);
    }

    std::unique_ptr<juce::XmlElement> CoreAudioProcessor::writeToXml() {
        auto rootElement = std::make_unique<juce::XmlElement>("Root");

        // Set the XML params version
        rootElement->setAttribute("SchemaVersion", PARAMS_SCHEMA_VERSION);

        // Store the parameters
        juce::XmlElement* paramsElement = rootElement->createNewChildElement("Params");
        for (const ParameterInterface& param : _paramsList) {
            paramsElement->setAttribute(param.name, param.getter());
        }

        for (const CustomParameterInterface& param : _customParamsList) {
            juce::XmlElement* thisParameterElement = paramsElement->createNewChildElement(param.name);
            param.writeToXml(thisParameterElement);
        }

        return rootElement;
    }

    void CoreAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
        std::unique_ptr<juce::XmlElement> element(getXmlFromBinary(data, sizeInBytes));
        restoreFromXml(std::move(element));
    }

    void CoreAudioProcessor::restoreFromXml(std::unique_ptr<juce::XmlElement> element) {
        // Parse the XML
        if (element != nullptr) {

            // If state was saved using an old plugin we need to migrate the XML data
            if (element->getIntAttribute("SchemaVersion", 0) < PARAMS_SCHEMA_VERSION) {
                element = _migrateParameters(std::move(element));
            }

            // Iterate through our list of parameters, restoring them from the XML attributes
            juce::XmlElement* paramsElement = element->getChildByName("Params");
            if (paramsElement != nullptr) {
                for (const ParameterInterface& param : _paramsList) {
                    if (paramsElement->hasAttribute(param.name)) {
                        param.setter(static_cast<float>(paramsElement->getDoubleAttribute(param.name)));
                    }
                }

                for (const CustomParameterInterface& param : _customParamsList) {
                    juce::XmlElement* thisParameterElement = paramsElement->getChildByName(param.name);

                    if (thisParameterElement != nullptr) {
                        param.restoreFromXml(thisParameterElement);
                    }
                }
            }
        }
    }

    void CoreAudioProcessor::registerParameter(juce::AudioParameterFloat*& param,
                                               const juce::String& name,
                                               const ParameterDefinition::RangedParameter<double>* range,
                                               float defaultValue,
                                               float precision) {
        param = new juce::AudioParameterFloat(name, name, {static_cast<float>(range->minValue), static_cast<float>(range->maxValue), precision}, defaultValue);

        ParameterInterface interface = {name,
                                        [&param]() { return param->get(); },
                                        [&](float val) { setParameterValueInternal(param, val); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    void CoreAudioProcessor::registerParameter(juce::AudioParameterFloat*& param,
                                               const juce::String& name,
                                               const ParameterDefinition::RangedParameter<float>* range,
                                               float defaultValue,
                                               float precision) {
        param = new juce::AudioParameterFloat(name, name, {static_cast<float>(range->minValue), static_cast<float>(range->maxValue), precision}, defaultValue);

        ParameterInterface interface = {name,
                                        [&param]() { return param->get(); },
                                        [&](float val) { setParameterValueInternal(param, val); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    void CoreAudioProcessor::registerParameter(juce::AudioParameterInt*& param,
                                               const juce::String& name,
                                               const ParameterDefinition::BaseParameter<int>* range,
                                               int defaultValue) {
        param = new juce::AudioParameterInt(name, name, range->minValue, range->maxValue, defaultValue);

        ParameterInterface interface = {name,
                                        [&param]() { return param->get(); },
                                        [&](float val) { setParameterValueInternal(param, static_cast<int>(val)); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    void CoreAudioProcessor::registerParameter(juce::AudioParameterBool*& param,
                                               const juce::String& name,
                                               bool defaultValue) {
        param = new juce::AudioParameterBool(name, name, defaultValue);

        ParameterInterface interface = {name,
                                        [&param]() { return param->get(); },
                                        [&](float val) { setParameterValueInternal(param, static_cast<bool>(val)); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        addParameter(param);
    }

    void CoreAudioProcessor::registerPrivateParameter(juce::AudioParameterFloat*& param,
                                                      const juce::String& name,
                                                      const ParameterDefinition::RangedParameter<double>* range,
                                                      float defaultValue,
                                                      float precision,
                                                      std::function<void(float)> setter) {
        param = new juce::AudioParameterFloat(name, name, {static_cast<float>(range->minValue), static_cast<float>(range->maxValue), precision}, defaultValue);

        ParameterInterface interface = {name,
                                        [&param]() { return param->get(); },
                                        [setter](float val) { setter(val); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        _privateParameters.push_back(param);
    }

    void CoreAudioProcessor::registerPrivateParameter(juce::AudioParameterInt*& param,
                                                      const juce::String& name,
                                                      const ParameterDefinition::BaseParameter<int>* range,
                                                      int defaultValue,
                                                      std::function<void(int)> setter) {
        param = new juce::AudioParameterInt(name, name, range->minValue, range->maxValue, defaultValue);

        ParameterInterface interface = {name,
                                        [&param]() { return param->get(); },
                                        [setter](float val) { setter(static_cast<int>(val)); }};
        _paramsList.push_back(interface);

        param->addListener(&_parameterBroadcaster);
        _privateParameters.push_back(param);
    }

    template <typename PARAM_TYPE>
    void CoreAudioProcessor::registerPrivateParameter(PARAM_TYPE*& param, const juce::String& name) {
        param = new PARAM_TYPE();

        CustomParameterInterface interface = {name,
                                              [&param](juce::XmlElement* element) { param->writeToXml(element); },
                                              [&param](juce::XmlElement* element) { param->restoreFromXml(element); }};
        _customParamsList.push_back(interface);

        param->setListener(&_parameterBroadcaster);
        _customParameters.push_back(param);
    }

    std::vector<float> CoreAudioProcessor::_stringToFloatVector(const juce::String sFloatCSV) const {
        juce::StringArray tokenizer;
        tokenizer.addTokens(sFloatCSV, ",","");

        std::vector<float> values;

        for (int iii {0}; iii < tokenizer.size(); iii++) {
            values.push_back(tokenizer[iii].getFloatValue());
        }

        return values;
    }

    std::unique_ptr<juce::XmlElement> CoreAudioProcessor::_migrateParameters(std::unique_ptr<juce::XmlElement> rootElement) {
        const int schemaVersion {rootElement->getIntAttribute("SchemaVersion", 0)};

        std::unique_ptr<juce::XmlElement> retVal = std::make_unique<juce::XmlElement>("Root");

        if (schemaVersion == 0) {
            // This is an original parameter schema - parameters are normalised values in a single string

            forEachXmlChildElement((*rootElement), childElement) {
                if (childElement->hasTagName("AllUserParam")) {

                    // Read the values into a float array
                    juce::String sFloatCSV = childElement->getAllSubText();
                    std::vector<float> readParamValues = _stringToFloatVector(sFloatCSV);
                    _migrateParamValues(readParamValues);

                    std::vector<juce::String> paramNames = _provideParamNamesForMigration();

                    juce::XmlElement* paramsElement = retVal->createNewChildElement("Params");

                    for (size_t idx {0}; idx < paramNames.size(); idx++) {
                        if (idx < readParamValues.size()) {
                            paramsElement->setAttribute(paramNames[idx], readParamValues[idx]);
                        }
                    }
                }
            }
        }

        return retVal;
    }
}
