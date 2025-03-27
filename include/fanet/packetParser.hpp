#pragma once

#include "fanet.hpp"
#include "etl/optional.h"
#include "etl/vector.h"
#include "etl/bit_stream.h"
#include "etl/variant.h"
#include "header.hpp"
#include "address.hpp"
#include "tracking.hpp"
#include "name.hpp"
#include "message.hpp"
#include "groundTracking.hpp"

namespace FANET
{
    /**
     * @brief A variant type to hold different payload types.
     * @tparam MAXFRAMESIZE The size of the message payload.
     * @tparam MAXFRAMESIZE The size of the name payload.
     */
    template <size_t MAXFRAMESIZE>
    using PayloadVariant = etl::variant<TrackingPayload, NamePayload<MAXFRAMESIZE>, MessagePayload<MAXFRAMESIZE>, GroundTrackingPayload, ServicePayload>;

    /**
     * @brief A class to parse FANET packets from a byte buffer.
     * 
     * The PacketParser class is responsible for parsing a byte buffer into a FANET packet.
     * It reads the header, source address, optional extended header, and payload from the buffer.
     * The class supports different payload types based on the message type in the header.
     * 
     * @tparam MAXFRAMESIZE The size of the message payload.
     * @tparam MAXFRAMESIZE The size of the name payload.
     */
    template <size_t MAXFRAMESIZE>
    class PacketParser final
    {
    public:
        /**
         * @brief Parse a byte buffer into a FANET packet.
         * 
         * This function reads the header, source address, optional extended header, and payload from the buffer.
         * It supports different payload types based on the message type in the header.
         * 
         * @param buffer The byte buffer containing the packet data.
         * @return The parsed FANET packet.
         */
        static Packet<MAXFRAMESIZE> parse(etl::span<const uint8_t> buffer)
        {
            etl::bit_stream_reader reader((uint8_t *)buffer.data(), buffer.size(), etl::endian::big);
            Header header;
            Address source;
            etl::optional<Address> optDestination;
            etl::optional<ExtendedHeader> optExtHeader;
            etl::optional<uint32_t> optSignature;
            etl::optional<PayloadVariant<MAXFRAMESIZE>> optPayload;

            reader.restart();
            header = Header::deserialize(reader);
            source = Address::deserialize(reader);
            uint16_t headerSize=4;
            if (header.extended())
            {
                optExtHeader = ExtendedHeader::deserialize(reader);
                headerSize += 1;
                if (optExtHeader->unicast())
                {
                    optDestination = Address::deserialize(reader);
                    headerSize += 3;
                }
                if (optExtHeader->signature())
                {
                    headerSize += 4;
                    optSignature = etl::reverse_bytes(reader.read_unchecked<uint32_t>());
                }
            }

            switch (header.type())
            {
            case Header::MessageType::TRACKING:
                optPayload = TrackingPayload::deserialize(reader);
                break;
            case Header::MessageType::NAME:
                optPayload = NamePayload<MAXFRAMESIZE>::deserialize(reader, buffer.size() - headerSize);
                break;
            case Header::MessageType::MESSAGE:
                optPayload = MessagePayload<MAXFRAMESIZE>::deserialize(reader, buffer.size() - headerSize);
                break;
            case Header::MessageType::GROUND_TRACKING:
                optPayload = GroundTrackingPayload::deserialize(reader);
                break;
            case Header::MessageType::SERVICE:
                optPayload = ServicePayload::deserialize(reader, buffer.size() - headerSize);
                break;
            default:
                break; // ACK or unsupported types
            }

            return Packet<MAXFRAMESIZE>(header, source, optDestination, optExtHeader, optSignature, optPayload);
        }
    };
};
