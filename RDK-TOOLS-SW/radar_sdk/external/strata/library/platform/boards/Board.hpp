/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <components/interfaces/IComponent.hpp>
#include <map>
#include <platform/interfaces/IBoard.hpp>

class Board :
    public IBoard
{
public:
    IComponent *getIComponent(uint8_t type, uint8_t id, uint8_t impl) override;
    IModule *getIModule(uint8_t type, uint8_t id, uint8_t impl) override;

protected:
    template <typename T>
    inline void registerComponent(T &component, uint8_t id)
    {
        const uint8_t type = T::getType();
        const uint16_t key = (type << 8) | id;
        m_components[key]  = &component;
    }

    template <typename T>
    inline void registerModule(T &module, uint8_t id)
    {
        const uint8_t type = T::getType();
        const uint16_t key = (type << 8) | id;
        m_modules[key]     = &module;
    }

private:
    std::map<uint16_t, IComponent *> m_components;
    std::map<uint16_t, IModule *> m_modules;
};
