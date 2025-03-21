# FANET Library

FANET (Flying Ad-hoc Network) is a lightweight, embedded networking library designed for aerial vehicles and ground stations. It provides a robust protocol for communication between flying objects and ground stations.

## Quick Start

```cpp
#include "fanet/fanet.hpp"

// Create a connector implementation
class MyConnector : public FANET::Connector {
    // Implement required methods...
};

// Initialize the protocol
MyConnector connector;
FANET::Protocol protocol(&connector);

// Set your device address
protocol.ownAddress(FANET::Address(0x01, 0x1234)); // Manufacturer ID: 0x01, Device ID: 0x1234
```

## Core Components

### Protocol (`protocol.hpp`)
The main protocol handler that manages packet transmission and reception. It handles:
- Packet forwarding
- Acknowledgments
- Neighbor discovery
- Transmission scheduling

```cpp
// Example: Sending a tracking packet
FANET::TrackingPayload payload;
payload.latitude(47.123).longitude(8.456).altitude(1000).speed(30.5);

auto packet = FANET::Packet<1>()
    .source(myAddress)
    .payload(payload)
    .singleHop();  // Request acknowledgment

protocol.sendPacket(packet);
```

### Address (`address.hpp`)
Represents a FANET device address consisting of:
- Manufacturer ID (8 bits)
- Unique Device ID (16 bits)

```cpp
FANET::Address addr(0x01, 0x1234);  // Mfg: 0x01, Device: 0x1234
```

### Packet Types

#### Tracking (`tracking.hpp`)
Used for real-time position reporting of aircraft:
- Position (lat/lon)
- Altitude
- Speed
- Aircraft type
- Climb rate
- Ground track
- Turn rate

```cpp
FANET::TrackingPayload tracking;
tracking.latitude(47.123)
        .longitude(8.456)
        .altitude(1000)
        .speed(30.5)
        .aircraftType(FANET::TrackingPayload::AircraftType::PARAGLIDER);
```

#### Ground Tracking (`groundTracking.hpp`)
Used for ground-based objects:
- Position
- Type (walking, vehicle, bike, etc.)
- Support status (need help, technical support, etc.)

```cpp
FANET::GroundTrackingPayload ground;
ground.latitude(47.123)
      .longitude(8.456)
      .groundType(FANET::GroundTrackingPayload::TrackingType::WALKING);
```

#### Message (`message.hpp`)
General-purpose message transmission:
- Text messages
- Custom data payloads
- Configurable size

```cpp
FANET::MessagePayload<100> msg;  // 100-byte message capacity
msg.message("Hello FANET!");
```

#### Name (`name.hpp`)
Device identification payload:
- Device name/identifier
- Configurable size

```cpp
FANET::NamePayload<20> name;  // 20-char name capacity
name.name("MyDevice");
```

### Supporting Classes

#### NeighbourTable (`neighbourTable.hpp`)
Maintains a list of nearby FANET devices:
- Address tracking
- Last seen timestamps
- Automatic cleanup

#### Zone (`zone.hpp`)
Manages regional settings:
- Frequency bands
- Power limits
- Geographic boundaries

```cpp
FANET::Zone zone;
auto region = zone.findZone(47.123, 8.456);  // Get region settings for coordinates
```

#### BlockAllocator (`blockAllocator.hpp`)
Memory management for packet transmission:
- Fixed-size memory pool
- Efficient allocation/deallocation
- Prevents fragmentation

## Advanced Features

### Packet Forwarding
FANET supports multi-hop packet forwarding:
```cpp
auto packet = FANET::Packet<1>()
    .source(myAddress)
    .destination(targetAddress)
    .payload(payload)
    .forward(true)    // Enable forwarding
    .twoHop();       // Request 2-hop acknowledgment
```

### Acknowledgments
Three acknowledgment modes:
- None: No acknowledgment
- Single-hop: Direct acknowledgment
- Two-hop: Forwarded acknowledgment

### Regional Compliance
The Zone system ensures compliance with regional regulations:
```cpp
FANET::Zone zone;
auto region = zone.findZone(latitude, longitude);
// Configure radio with region.mac settings
```

## Implementation Requirements

To use FANET, implement the `Connector` interface:
```cpp
class MyConnector : public FANET::Connector {
public:
    uint32_t fanet_getTick() const override {
        return /* your timestamp implementation */;
    }

    bool fanet_sendFrame(uint8_t codingRate, etl::span<const uint8_t> data) override {
        return /* your radio transmission implementation */;
    }

    void fanet_ackReceived(uint16_t id) override {
        /* handle received acknowledgments */
    }
};
```

## Dependencies
- ETL (Embedded Template Library)
- C++17 or later

## Notes
- All memory allocations are static
- Designed for embedded systems
- Uses template metaprogramming for compile-time optimizations
- Supports various payload sizes through templates

## Best Practices
1. Always check regional compliance using the Zone system
2. Implement proper error handling in your Connector
3. Monitor neighbor table size in dense networks
4. Use appropriate acknowledgment modes based on reliability needs
5. Consider airtime restrictions when sending frequent updates
