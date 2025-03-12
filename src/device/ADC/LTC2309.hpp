#pragma once

#include <map>
#include <cstidint>

class LTC2309
{
private:
    uint8_t       i2c_addr_;
    bool          is_single_ended_;
    QueueHandle_t req_queue_;

    PSI2CReq psi2c_req_;
    std::map<int, int> chan_assign_;  // stores <variable:channel>
                                      // defined by detector and passed to the constructor

public:
    LTC2309( uint8_t i2c_addr
           , bool is_single_ended
           , psi2c_req_queue
           , std::map<int, char> chan_assign);

    ~LTC2309() = default;

    void read( uint8_t chan );
};
