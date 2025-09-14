#include "PluginModulationInterface.h"
#include "GraphViewComponent.h"
#include "ModelInterface.hpp"

namespace {
    juce::Array<juce::AudioProcessorParameter*> getParamsExcludingSelected(
            const juce::Array<juce::AudioProcessorParameter*>& pluginParameters,
            PluginModulationConfig config) {
        // Get the list of all parameters, and create a subset that includes only ones that haven't
        // been selected yet
        // TODO this might break if the plugin changes its list of parameters after this list has
        // been created
        juce::Array<juce::AudioProcessorParameter*> availableParameters;

        for (juce::AudioProcessorParameter* thisParam : pluginParameters) {
            bool shouldCopy {true};

            // If this parameter name is already in the config, don't add it to the list
            for (const std::shared_ptr<PluginParameterModulationConfig> paramConfig : config.parameterConfigs) {
                const juce::String thisParamName = thisParam->getName(PluginParameterModulationConfig::PLUGIN_PARAMETER_NAME_LENGTH_LIMIT);
                if (thisParamName == paramConfig->targetParameterName) {
                    shouldCopy = false;
                    break;
                }
            }

            if (shouldCopy) {
                availableParameters.add(thisParam);
            }
        }

        return availableParameters;
    }
}

PluginModulationInterface::PluginModulationInterface(SyndicateAudioProcessor& processor)
    : _processor(processor) {
}

PluginModulationConfig PluginModulationInterface::getPluginModulationConfig(int chainNumber, int pluginNumber) {
    return ModelInterface::getPluginModulationConfig(_processor.manager, chainNumber, pluginNumber);
}

void PluginModulationInterface::togglePluginModulationActive(int chainNumber, int pluginNumber) {
    PluginModulationConfig config = ModelInterface::getPluginModulationConfig(_processor.manager, chainNumber, pluginNumber);
    _processor.setPluginModulationIsActive(chainNumber, pluginNumber, !config.isActive);
}

void PluginModulationInterface::addSourceToTarget(int chainNumber, int pluginNumber, int targetNumber, ModulationSourceDefinition source) {
    _processor.addModulationSourceToTarget(chainNumber, pluginNumber, targetNumber, source);
}

void PluginModulationInterface::removeSourceFromTarget(int chainNumber, int pluginNumber, int targetNumber, ModulationSourceDefinition source) {
    _processor.removeModulationSourceFromTarget(chainNumber, pluginNumber, targetNumber, source);
}

void PluginModulationInterface::setModulationTargetValue(int chainNumber, int pluginNumber, int targetNumber, float val) {
    _processor.setModulationTargetValue(chainNumber, pluginNumber, targetNumber, val);
}

void PluginModulationInterface::setModulationSourceValue(int chainNumber, int pluginNumber, int targetNumber, int sourceNumber, float val) {
    _processor.setModulationSourceValue(chainNumber, pluginNumber, targetNumber, sourceNumber, val);
}

void PluginModulationInterface::selectModulationTarget(int chainNumber, int pluginNumber, int targetNumber) {
    // Collect the parameter list for this plugin
    std::shared_ptr<juce::AudioPluginInstance> plugin =
        ModelInterface::getPlugin(_processor.manager, chainNumber, pluginNumber);

    if (plugin != nullptr) {
        // Create the selector
        const PluginModulationConfig config = ModelInterface::getPluginModulationConfig(_processor.manager, chainNumber, pluginNumber);

        const bool isReplacingParameter {targetNumber < config.parameterConfigs.size()};

        PluginParameterSelectorListParameters parameters {
            _processor.pluginParameterSelectorState,
            getParamsExcludingSelected(plugin->getParameters(), config),
            [&, chainNumber, pluginNumber, targetNumber](juce::AudioProcessorParameter* parameter, bool shouldClose) { _onPluginParameterSelected(parameter, chainNumber, pluginNumber, targetNumber, shouldClose); },
            isReplacingParameter
        };

        const juce::String title = isReplacingParameter
            ? "Replacing parameter " + config.parameterConfigs[targetNumber]->targetParameterName
            : "New parameter for plugin " + plugin->getPluginDescription().name;

        _parameterSelectorWindow.reset(new PluginParameterSelectorWindow(
            [&]() { _parameterSelectorWindow.reset(); }, parameters, title));

        _parameterSelectorWindow->takeFocus();
    }
}

void PluginModulationInterface::removeModulationTarget(int chainNumber, int pluginNumber, int targetNumber) {
    _processor.removeModulationTarget(chainNumber, pluginNumber, targetNumber);
}

juce::AudioProcessorParameter* PluginModulationInterface::getPluginParameterForTarget(int chainNumber, int pluginNumber, int targetNumber) {
    juce::AudioProcessorParameter* retVal {nullptr};

    PluginModulationConfig config = ModelInterface::getPluginModulationConfig(_processor.manager, chainNumber, pluginNumber);

    if (config.parameterConfigs.size() > targetNumber) {
        std::shared_ptr<juce::AudioPluginInstance> plugin = ModelInterface::getPlugin(_processor.manager, chainNumber, pluginNumber);

        if (plugin != nullptr) {
            const juce::String paramName(config.parameterConfigs[targetNumber]->targetParameterName);

            const juce::Array<juce::AudioProcessorParameter*>& parameters = plugin->getParameters();
            for (juce::AudioProcessorParameter* thisParam : parameters) {
                if (thisParam->getName(PluginParameterModulationConfig::PLUGIN_PARAMETER_NAME_LENGTH_LIMIT) == paramName) {
                    retVal = thisParam;
                    break;
                }
            }
        }
    }

    return retVal;
}

void PluginModulationInterface::_onPluginParameterSelected(juce::AudioProcessorParameter* parameter, int chainNumber, int pluginNumber, int targetNumber, bool shouldClose) {
    _processor.setModulationTarget(chainNumber, pluginNumber, targetNumber, parameter->getName(PluginParameterModulationConfig::PLUGIN_PARAMETER_NAME_LENGTH_LIMIT));
    _parameterSelectorWindow.reset();

    if (!shouldClose) {
        selectModulationTarget(chainNumber, pluginNumber, targetNumber + 1);
    }
}
