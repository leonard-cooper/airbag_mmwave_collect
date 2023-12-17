#include "BoardHelper.hpp"

#include <platform/ethernet/BoardEthernetTcp.hpp>
#include <platform/exception/EConnection.hpp>


std::unique_ptr<BoardInstance> createUsbBoard(uint16_t vid, const std::vector<uint16_t> &pids)
{
    const bool serial   = false;
    const bool usb      = true;
    const bool ethernet = false;
    const bool wiggler  = false;
    BoardManager boardManager(serial, ethernet, usb, wiggler);

    if (boardManager.enumerate() == 0U)
    {
        return nullptr;
    }

    for (uint16_t pid : pids)
    {
        try
        {
            auto board = boardManager.createBoardInstance(vid, pid);
            if (board)
            {
                return board;
            }
        }
        catch (const EConnection &)
        {
            // Among others an exception is thrown when the board is not found.
            // We will try the other PIDs and will make sure that no exceptions escape here.
        }
    }

    return nullptr;
}

std::unique_ptr<BoardInstance> createTcpBoard(uint16_t vid, const std::vector<uint16_t> &pids, ipAddress_t ipAddress)
{
    auto board = BoardEthernetTcp::createBoardInstance(ipAddress);
    if (!board || board->getVid() != vid)
        return nullptr;

    auto it = std::find_if(pids.begin(), pids.end(), [&](uint16_t pid) {
        return board->getPid() == pid;
    });
    if (it == pids.end())
        return nullptr;

    return board;
}
