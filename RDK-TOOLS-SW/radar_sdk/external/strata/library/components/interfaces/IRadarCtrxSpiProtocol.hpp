#pragma once

#include <Definitions.hpp>
#include <platform/interfaces/access/ISpi.hpp>

#include <components/interfaces/IRadarCtrxSpiStructs.hpp>

// forward declarations
class CtrxSpiFrameSend;
class CtrxSpiFrameReceive;
class CtrxSpiFwCommand;


/**
 * @brief The ICtrxSpiProtocol wrapper interface adds CTRX specifics to the given ISpi
 */
class ICtrxSpiProtocol
{
public:
    /**
    * @brief CTRX Check Types for SPI frames
    */
    enum class ConfigType : uint8_t
    {
        CrcCheck = 0,  //!< CRCs in SPI frames shall be checked or not, defaults to true
        CriCheck = 1,  //!< CRIs in SPI frames shall be checked or not, defaults to true
    };

    STRATA_API virtual ~ICtrxSpiProtocol() = default;
    /**
    * @brief Configure error handling functionality
    * @note If the function is never called for one setting, the default value is used. Each setting has an own default.
    * @param type The config type to enable/disable
    * @param enable True to enable the setting, false to disable
    */
    virtual void configureErrorHandling(ConfigType type, bool enable) = 0;
    /**
    * @brief Read data from an SPI device in half-duplex considering wait for RFT
    *
    * @param count number of words to be read
    * @param buffer a buffer of the specified length
    */
    virtual void read(uint32_t count, uint32_t buffer[]) = 0;

    /**
    * @brief Write data to an SPI device in half-duplex mode considering wait for RFT
    *
    * @param count number of words to be written
    * @param buffer a buffer of the specified length
    */
    virtual void write(uint32_t count, const uint32_t buffer[]) = 0;

    /**
    * @brief Read and write data from / to an SPI device in full-duplex mode considering wait for RFT
    *
    * @param count number of words to be read / written
    * @param bufWrite a buffer of the specified length with the data to write
    * @param bufRead a buffer of the specified length where the read data will be written to
    */
    virtual void transfer(uint32_t count, const uint32_t bufWrite[], uint32_t bufRead[]) = 0;

    /**
     * @brief Obtain the current setting of the maximum number of payload words in an SPI frame when reading
     * @return The maximum nuber of payload words
     */
    virtual uint8_t getMaxPayloadRead() const = 0;

    /**
     * @brief Obtain the current setting of the maximum number of payload words in an SPI frame when writing
     * @return The maximum nuber of payload words
     */
    virtual uint8_t getMaxPayloadWrite() const = 0;

    /**
     * @brief Send a frame and read the result
     * @note This function implements the half duplex communication for sending an SPI frame and then receiving the reply.
     * @note Upon error an exception will be thrown
     * @param sendFrame The frame to send
     * @param receiveFrame The received frame will be put here
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
    */
    virtual void sendReceive(const CtrxSpiFrameSend &sendFrame, CtrxSpiFrameReceive &receiveFrame, int32_t readCount = -1) = 0;

    /**
     * @brief Execute a firmware command
     * @note It is an abstraction on the sendReceive function to make handling easier for all the FW command functions.
     * @note They don't need to think about SPI frames, just about the FW command and its parameters.
     * @param command The command to execute
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     */
    virtual void executeSpiFwCommand(CtrxSpiFwCommand &command, int32_t readCount = -1) = 0;

    /**
     * @brief Send and receive generic SPI frames in full duplex mode
     * @note param0 and the payload length are stored at the same place in the frame and therefore they can not be
     *       set at the same time. Because of that fact the param0 parameter will be ignored if paramsCount > 0.
     * @param opcode The OpCode to put into the header
     * @param param0 The value of the parameter to set, if paramsCount is > 0 param0 will be ignored
     * @param params The data to write to the payload
     * @param paramsCount The number of words to write to the payload
     * @param wordCount The number of expected words to read including the header and CRC32, if the wordCount is higher than the paramsCount (+ header,...) the Frame will be padded with 0.
     *        If wordCount is less then the send frame length, sendSpiRequest automatically reads the same amount of words as it sends out.
     *        sendSpiRequest reads at least 3 words even if the outgoing message is header only because thats the minimum response frame size
     * @return SpiResult structure containing information regarding the received response
     */
    virtual CtrxSpiStructs::SpiResult sendSpiRequest(uint8_t opcode, uint8_t param0, const uint32_t params[], uint8_t paramsCount, uint8_t wordCount) = 0;

    /**
     * @brief Send a Do_NOP command to retrieve the last response
     * rcvFrame Will be filled with the received data
     * @param param0 ignored (don't care)
     * @param readCount The number of words to read. This number has to include the header and the trailing CRC32, if used.
     *                  The readCount parameter will be ignored if the value is <0 and the amount of words to read will be set automatically.
     * @return The information if a valid frame was received
     */
    virtual uint8_t doNop(CtrxSpiFrameReceive &rcvframe, uint8_t param0 = 0, int32_t readCount = -1) = 0;
    // Originally it was intended to have CtrxSpiFrameReceive::Validity as return type here.
    // But this needs to include CtrxSpiFrameReceive.hpp which is located in the CTRX package and cannot be used in strata core.
    // Once the CTRX-specific interfaces are moved to the CTRX package, this shall be changed back.
    // See also https://jirard.intra.infineon.com/browse/STRATA-40

    /**
     * @brief Retrieve the raw data of the last received response upon error.
     * @note The last response is only stored if an error occurred. If no error occurred, you will get the last error response.
     * @param count Reference parameter, where the number of words in the response is stored
     * @return Pointer to the response data
     */
    virtual const uint32_t *getLastErrorResponse(uint8_t &count) const = 0;
};
