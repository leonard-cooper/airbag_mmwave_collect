/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "BoardRemote.hpp"

#include <common/cpp11/memory.hpp>
#include <platform/exception/EConnection.hpp>

#include <protocol/remote/RemoteModuleRadar.hpp>
#include <protocol/remote/RemotePowerAmplifier.hpp>
#include <protocol/remote/RemotePowerSupply.hpp>
#include <protocol/remote/RemoteProcessingRadar.hpp>
#include <protocol/remote/RemoteRadarAtr22.hpp>
#include <protocol/remote/RemoteRadarAvian.hpp>
#include <protocol/remote/RemoteRadarLtr11.hpp>
#include <protocol/remote/RemoteRadarRxs.hpp>


BoardRemote::BoardRemote(IBridge *bridge) :
    m_bridge {bridge}
{
    m_commands = bridge->getIBridgeControl()->getIVendorCommands();
    if (m_commands == nullptr)
    {
        throw EConnection("IVendorCommands not implemented in this Bridge");
    }
}

std::unique_ptr<IComponent> BoardRemote::createComponent(uint8_t type, uint8_t id, uint8_t impl)
{
    switch (type)
    {
        case COMPONENT_TYPE_RADAR:
            switch (impl)
            {
                case COMPONENT_IMPL_RADAR_RXS:
                    return std::make_unique<RemoteRadarRxs>(m_commands, id);
                    break;
                case COMPONENT_IMPL_RADAR_AVIAN:
                    return std::make_unique<RemoteRadarAvian>(m_commands, id);
                    break;
                case COMPONENT_IMPL_RADAR_ATR22:
                    return std::make_unique<RemoteRadarAtr22>(m_commands, id);
                    break;
                case COMPONENT_IMPL_RADAR_LTR11:
                    return std::make_unique<RemoteRadarLtr11>(m_commands, id);
                    break;
                default:
                    return std::make_unique<RemoteRadar>(m_commands, id);
                    break;
            }
            break;
        case COMPONENT_TYPE_PROCESSING_RADAR:
            return std::make_unique<RemoteProcessingRadar>(m_commands, id);
            break;
        case COMPONENT_TYPE_POWER_SUPPLY:
            return std::make_unique<RemotePowerSupply>(m_commands, id);
            break;
        case COMPONENT_TYPE_POWER_AMPLIFIER:
            return std::make_unique<RemotePowerAmplifier>(m_commands, id);
            break;
        default:
            break;
    }

    return std::unique_ptr<IComponent>();
}

std::unique_ptr<IModule> BoardRemote::createModule(uint8_t type, uint8_t id, uint8_t /*impl*/)
{
    switch (type)
    {
        case MODULE_TYPE_RADAR:
            return std::make_unique<RemoteModuleRadar>(m_bridge, id);
            break;
        default:
            break;
    }

    return std::unique_ptr<IModule>();
}

IComponent *BoardRemote::getIComponent(uint8_t type, uint8_t id, uint8_t impl)
{
    const uint16_t key = (type << 8) | id;
    auto entry         = m_components.find(key);
    if (entry != m_components.end())
    {
        auto &component = entry->second;
        if ((impl != COMPONENT_IMPL_DEFAULT) && (component.second == COMPONENT_IMPL_DEFAULT))
        {
            //A specific instance was requested but we only have a default one => move to replaced vector
            m_replacedComponents.push_back(std::move(component.first));
            m_components.erase(key);
            //New component is inserted below
        }
        else
        {
            return component.first.get();
        }
    }
    //If we come here, we haven't found the component or it shall be replaced
    auto newComponent = createComponent(type, id, impl);
    IComponent *ptr   = newComponent.get();
    if (ptr)
    {
        ComponentImpl &newComponentImpl = m_components[key];  //Creates a new element in place and returns the reference
        newComponentImpl.first          = std::move(newComponent);
        newComponentImpl.second         = impl;
    }
    return ptr;
}

IModule *BoardRemote::getIModule(uint8_t type, uint8_t id, uint8_t impl)
{
    const uint16_t key = (type << 8) | id;
    auto entry         = m_modules.find(key);
    if (entry != m_modules.end())
    {
        auto &module = entry->second;
        if ((impl != MODULE_IMPL_DEFAULT) && (module.second == MODULE_IMPL_DEFAULT))
        {
            //A specific instance was requested but we only have a default one => move to replaced vector
            m_replacedModules.push_back(std::move(module.first));
            m_modules.erase(key);
            //New module is inserted below
        }
        else
        {
            return module.first.get();
        }
    }
    //If we come here, we haven't found the module or it shall be replaced
    auto newModule = createModule(type, id, impl);
    IModule *ptr   = newModule.get();
    if (ptr)
    {
        ModuleImpl &newModuleImpl = m_modules[key];  //Creates a new element in place and returns the reference
        newModuleImpl.first       = std::move(newModule);
        newModuleImpl.second      = impl;
    }
    return ptr;
}
