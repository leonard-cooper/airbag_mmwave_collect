/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

//#include <platform/interfaces/IBridgeData.hpp>
//#include <platform/interfaces/IBridgeControl.hpp>

#include <Definitions.hpp>
#include <components/interfaces/IComponent.hpp>
#include <modules/interfaces/IModule.hpp>
#include <modules/interfaces/IModuleRadar.hpp>

#include <cstdint>


class IBoard
{
public:
    STRATA_API virtual ~IBoard() = default;

    virtual IModule *getIModule(uint8_t type, uint8_t id, uint8_t impl = MODULE_IMPL_DEFAULT)          = 0;
    virtual IComponent *getIComponent(uint8_t type, uint8_t id, uint8_t impl = COMPONENT_IMPL_DEFAULT) = 0;

    template <class ComponentType>
    ComponentType *getComponent(uint8_t id)
    {
        return dynamic_cast<ComponentType *>(getIComponent(ComponentType::getType(), id, ComponentType::getImplementation()));
    }

    template <class ModuleType>
    ModuleType *getModule(uint8_t id = 0)
    {
        return dynamic_cast<ModuleType *>(getIModule(ModuleType::getType(), id, ModuleType::getImplementation()));
    }
};
