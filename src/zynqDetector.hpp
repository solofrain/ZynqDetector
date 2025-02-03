#pragma once

// C++ includes.
#include <vector>
#include <cstdint>
#include <memory>
// FreeRTOS includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "FreeRTOS_IP.h"
#include "network_interface.h"
#include "ff.h"  // FatFS file system library for SD card
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"

#include "msg.hpp"
#include "fpga.hpp"
#include "network.hpp"

typedef uint32_t reg_addr_t;

// Operations
const uint8_t REG_WR  { 0 };
const uint8_t REG_RD  { 1 };
const uint8_t SPI_WR  { 2 };
const uint8_t SPI_RD  { 3 };
const uint8_t I2C_WR  { 4 };
const uint8_t I2C_RD  { 5 };
const uint8_t ASIC_WR { 6 };
const uint8_t ASIC_RD { 7 };

const uint16_t UDP_MSG_PREAMBLE = 0x5053;
// Message coding
typedef struct {
    uint16_t  preamble;
    uint16_t  op;
    uint32_t  payload[ MAX_UDP_MSG_LENG - 1 ];
} udp_msg_t;

const std::vector<msg_code> msg_codec
    { msg_code { 0x0, 4, 0, 0 }
    , msg_code { 0x1, 4, 0, 4 }
    };

// Peripheral device definitions
typedef uint8_t interface_type_t;
const interface_type_t I2C = 0;
const interface_type_t SPI = 1;

typedef uint8_t device_t;
const device_t AD5254 = 0;
const device_t AD5593 = 1;
const device_t AD9249 = 2;


#define NORMAL               0x0
#define NETWORK_INIT_FAILURE 0x1
#define NETWORK_TX_FAILURE   0x2
#define NETWORK_RX_FAILURE   0x3

typedef struct
{
    device_t  device;
    uint8_t   addr;
} device_descriptor_t;

// device interface descriptor. Used for interface access in slow_access_task
typedef struct
{
    uint8_t if_type; // interface type: I2C, SPI, etc
    uint8_t if_no;   // interface number: 0, 1, ... to be used in combination with interf_type
    uint8_t if_data_reg;   // data register of the interface
    uint8_t if_instr_reg;  // instruction register for the interface
    std::vector<device_descriptor_t> dev_desc_v {};  // vector of devices connected to the interface
} interface_descriptor_t;

std::vector<device_descriptor_t> dev_desc_v{};

std::vector<interface_descriptor_t> if_desc_v{};

void device_registration( std::vector<device_interface_t> dev_if_v& )
{
    dev_if_v.push_back({ IF_TYPE_I2C, 0, {AD5254, AD}})
}


class ZynqDetector
{
protected:

    // Data types
    typedef struct
    {
        uint8_t  op;
        uint32_t addr;
    } fast_access_resp_t;

    // Queues
    const uint16_t FAST_ACCESS_RESP_QUEUE_LENG = 100;
    const uint16_t FAST_ACCESS_RESP_QUEUE_SIZE = FAST_ACCESS_RESP_QUEUE_LENG * sizeof(fast_access_resp_t);

    axi_reg reg;
    std::unique_ptr<Network> net;

    // Tasks
    static void udp_rx_task( void *pvParameters );
    static void udp_tx_task( void *pvParameters );
    // Task handlers
    TaskHandle_t  udp_rx_task_handle;
    TaskHandle_t  udp_tx_task_handle;
    
    // msg_id parsers
    void access_mode_decode( msg_id_t msg_id );
    reg_addr_t fast_access_parse( msg_id_t msg_id );
    
    QueueHandle_t fast_access_resp_queue = NULL;

    
    QueueSetMemberHandle_t active_slow_req_queue;
    QueueSetMemberHandle_t active_resp_queue;

    TimerHandle_t xPollTimer = NULL;

    std::vector<uint32_t> poll_list{};  // PVs to be polled

    virtual void queue_init();
    virtual void create_tasks();

    // Network related
    bool string_to_addr( const std::string& addr_str, uint8_t* addr );
    void read_network_config( const std::string& filename );
    void network_init();

public:

    ZynqDetector();
    ~ZynqDetector();

    // Set failure number to register.
    void set_fail_num( uint32_t fail_num );

    template <typename T>
    void ZynqDetector::report_error( const std::string& s, T err_code, uint32_t fail_num );

    void Network::init

};
