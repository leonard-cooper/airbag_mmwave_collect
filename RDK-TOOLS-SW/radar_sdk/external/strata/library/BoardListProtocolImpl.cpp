/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include <platform/BoardListProtocol.hpp>
#include <platform/templates/boardFactoryFunction.hpp>

#include <platform/fpga/BridgeFpgaIrpli.hpp>

#include <platform/boards/BoardGeneric.hpp>
#include <platform/boards/BoardRemote.hpp>


// only UVC or CDC boards need to be listed to be distinguished from non-strata boards,
// or any board with a special bridge factory
const BoardData BoardListProtocolArray[] {
    // UVC
    {0x04b4, 0xc0f3, BoardFactoryFunction<BoardGeneric>},  // FX3 Board
    {0x04b4, 0xc0c3, BoardFactoryFunction<BoardGeneric>},  // CX3 Board
    {0x04b4, 0xa0c3, BoardFactoryFunction<BoardGeneric>},  // Animator Board

    {0x04b4, 0xc0f4, BoardFactoryFunctionWrapped<BridgeFpgaIrpli, BoardGeneric>},  // FPGA LVDS data capture board

    // CDC
    {0x058b, 0x0251, BoardFactoryFunction<BoardRemote>},  // Avian on Hatvan (Atmel Sams)
    {0x058b, 0x0252, BoardFactoryFunction<BoardRemote>},  // Avian on RBB (Aurix)
    {0x058b, 0x0253, BoardFactoryFunction<BoardRemote>},  // Avian on V9 (XMC)

};


const BoardData::const_iterator BoardListProtocol::begin = std::begin(BoardListProtocolArray);
const BoardData::const_iterator BoardListProtocol::end   = std::end(BoardListProtocolArray);
