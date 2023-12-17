/**
 * @copyright 2020 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Board.hpp"


IComponent *Board::getIComponent(uint8_t type, uint8_t id, uint8_t /*impl*/)
{
    const uint16_t key = (type << 8) | id;
    auto component     = m_components.find(key);
    if (component != m_components.end())
    {
        return component->second;
    }
    else
    {
        return nullptr;
    }
}

IModule *Board::getIModule(uint8_t type, uint8_t id, uint8_t /*impl*/)
{
    const uint16_t key = (type << 8) | id;
    auto module        = m_modules.find(key);
    if (module != m_modules.end())
    {
        return module->second;
    }
    else
    {
        return nullptr;
    }
}
