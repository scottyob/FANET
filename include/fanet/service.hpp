#pragma once

#include <stdint.h>
#include <math.h>
#include "etl/algorithm.h"
#include "header.hpp"

namespace FANET
{
    /**
     * Tracking payload
     * Messagetype : 1
     */
    class ServicePayload final : public Payloadbase
    {
    public:
        uint8_t header;
        uint8_t eHeader;
        int32_t latitudeRaw = 0;
        int32_t longitudeRaw = 0;
        uint16_t altitudeRaw = 0;
        uint8_t temperatureRaw;
        uint8_t windHeadingRaw;
        bool sWindBit;
        uint8_t windSpeedRaw;
        bool gWindBit;
        uint8_t windGustRaw;
        uint8_t humidityRaw;
        uint16_t barometricRaw;

    public:
        /**
         * @brief Default constructor.
         */
        explicit() = default;

        /**
         * @brief Get the message type.
         * @return The message type.
         */
        Header::MessageType type() const
        {
            return Header::MessageType::SERVICE;
        }

        bool hasGateway()
        {
            return header & 0x80;
        }
        bool hasTemperature()
        {
            return header & 0x40;
        }
        bool hasWind()
        {
            return header & 0x20;
        }
        bool hasHumidity()
        {
            return header & 0x10;
        }
        bool hasBarometric()
        {
            return header & 0x08;
        }
        bool haseHeader()
        {
            return header & 0x01;
        }

        /**
         * @brief Get the latitude in degrees.
         * @return The latitude in degrees.
         */
        float latitude() const
        {
            return ((latitudeRaw << 8) >> 8) / 93206.f;
        }

        /**
         * @brief Get the longitude in degrees.
         * @return The longitude in degrees.
         */
        float longitude() const
        {
            return ((longitudeRaw << 8) >> 8) / 46603.f;
        }

        /**
         * @brief Set the latitude in degrees.
         * @param lat The latitude in degrees.
         * @return Reference to the current object.
         */
        ServicePayload &latitude(float lat)
        {
            lat = etl::clamp(lat, -90.f, 90.f);
            latitudeRaw = roundf(lat * 93206.0f);
            return *this;
        }

        /**
         * @brief Set the longitude in degrees.
         * @param lon The longitude in degrees.
         * @return Reference to the current object.
         */
        ServicePayload &longitude(float lon)
        {
            lon = etl::clamp(lon, -180.f, 180.f);
            longitudeRaw = roundf(lon * 46603.0f);
            return *this;
        }

                /**
         * @brief get the temperature in degrees
         * @param The temperature
         */
        float temperature() {
            return static_cast<float>(temperatureRaw/2) - 64.0f;
        }


        /**
         * @brief Get the ground track in degrees.
         * @return The ground track in degrees.
         */
        float windHeading() const
        {
            return static_cast<float>(windHeadingRaw) * 360.f / 256.f;
        }

        /**
         * @brief Get the speed in kilometers per hour.
         * @return The speed in kilometers per hour.
         */
        float windSpeed() const
        {
            return sWindBit ? windSpeedRaw : windSpeedRaw / 5.0f;
        }

        /**
         * @brief Set the speed in kilometers per hour.
         * @param speed The speed in kilometers per hour.
         * @return Reference to the current object.
         */
        ServicePayload &winSpeed(float speed)
        {
            int32_t speed2 = etl::clamp(static_cast<int>(roundf(speed * 5)), 0, 127);
            if (speed2 > 127)
            {
                windSpeedRaw = speed2 / 5;
                sWindBit = true;
            }
            else
            {
                windSpeedRaw = speed2;
            }
            return *this;
        }

        /**
         * @brief Serialize the tracking payload to a bit stream.
         * @param writer The bit stream writer.
         */
        virtual void serialize(etl::bit_stream_writer &writer) const override
        {
           
        }

        /**
         * @brief Deserialize the service payload from a bit stream.
         * @param reader The bit stream reader.
         * @return The deserialized service payload.
         */
        static const ServicePayload deserialize(etl::bit_stream_reader &reader)
        {
            ServicePayload service;
           
            return service;
        }
    };

}