/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <platform/interfaces/IBoard.hpp>
#include <platform/interfaces/IBridge.hpp>

#include <map>
#include <memory>
#include <utility>
#include <vector>


class BoardRemote :
    public IBoard
{
public:
    BoardRemote(IBridge *bridge);

    IComponent *getIComponent(uint8_t type, uint8_t id, uint8_t impl) override;
    IModule *getIModule(uint8_t type, uint8_t id, uint8_t impl) override;

private:
    IBridge *m_bridge;
    IVendorCommands *m_commands;

    std::unique_ptr<IComponent> createComponent(uint8_t type, uint8_t id, uint8_t impl);
    std::unique_ptr<IModule> createModule(uint8_t type, uint8_t id, uint8_t impl);

    //If a component / module type  is requested for the first time, it is instantiated and put into
    //the map. In case the default type is requested first and then the specific type,
    //the default type would have to be replaced by the specific type.
    //But the user still has a reference to the default object, so it has to stay alive.
    //Therefore it is moved to m_replaced... vector, if this case occurs.

    using ComponentImpl = std::pair<std::unique_ptr<IComponent>, uint8_t>;
    std::map<uint16_t, ComponentImpl> m_components;
    std::vector<std::unique_ptr<IComponent>> m_replacedComponents;

    using ModuleImpl = std::pair<std::unique_ptr<IModule>, uint8_t>;
    std::map<uint16_t, ModuleImpl> m_modules;
    std::vector<std::unique_ptr<IModule>> m_replacedModules;
};
