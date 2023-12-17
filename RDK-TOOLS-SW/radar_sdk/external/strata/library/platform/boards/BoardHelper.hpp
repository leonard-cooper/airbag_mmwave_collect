#pragma once

#include "platform/BoardInstance.hpp"
#include "platform/BoardManager.hpp"
#include "platform/interfaces/link/ISocket.hpp"
#include <algorithm>

///
/// \brief Helper function to create a Usb Board object
///
/// \param vid  VID of the connected board
/// \param pids List of possible PIDs for the connected board, the first match is used.
/// \return std::unique_ptr<BoardInstance>
///
std::unique_ptr<BoardInstance> createUsbBoard(uint16_t vid, const std::vector<uint16_t> &pids);

///
/// \brief Helper function to create a Tcp Board object
///
/// \param vid          VID of the connected board
/// \param pids         List of possible PIDs for the connected board, the first match is used.
/// \param ipAddress    ip address of the connected board
/// \return std::unique_ptr<BoardInstance>
///
std::unique_ptr<BoardInstance> createTcpBoard(uint16_t vid, const std::vector<uint16_t> &pids, ipAddress_t ipAddress);
