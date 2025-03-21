#pragma once

#include "connector.hpp"
#include "etl/span.h"

namespace FANET
{

    class Connector
    {

    public:
        virtual ~Connector() {}

        /**
         * @brief return a number of ms. Does not matter if it's from epoch, as long as it's monotonic.
         */
        virtual uint32_t fanet_getTick() const = 0;

        virtual bool fanet_sendFrame(uint8_t codingRate, etl::span<const uint8_t> data) = 0;

        virtual void fanet_ackReceived(uint16_t id) = 0;
    };

}
