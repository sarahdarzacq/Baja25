#include "constants.h"
#include <FlexCAN_T4.h>
#include <can_bus.h>
#include <string>

FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16>  CAN_BUS::flexcan_bus;

CAN_BUS::CAN_BUS()
{
  flexcan_bus.begin();
  CANFD_timings_t config;
  config.clock = CLK_24MHz;
  config.baudrate = FLEXCAN_BAUD_RATE;
  config.baudrateFD = FLEXCAN_BAUD_RATE;
  config.propdelay = 190;
  config.bus_length = 1;
  config.sample = 75;
  flexcan_bus.setBaudRate(config);
  //flexcan_bus.setMB(FLEXCAN_MAX_MAILBOX);
  flexcan_bus.enableFIFO();
  flexcan_bus.enableMBInterrupts();
  flexcan_bus.onReceive(can_parse);
}
/**
 * @brief Send a command
 * @param cmd_id ODrive CAN command ID number
 * @param remote RTR bit for CAN message (typically 0 for setters and 1 for
 * requesters)
 * @param buf 8-wide array of command data bytes
 * @return Status code of command send
 */
u8 CAN_BUS::send_command(u32 func_id, u32 node_id, bool remote, u8 buf[]) {
  // TODO: Fix error mwessages
  CANFD_message_t msg;

  if (func_id < 0x00 || 0x1f < func_id) {
    return 404;
  }

  msg.id = (node_id << 5) | func_id;
  msg.len = 64;
  memcpy(&msg.buf, buf, 64);
  //msg.flags.remote = remote;

  int write_code = flexcan_bus.write(msg);
  if (write_code == -1) {
    return 405;
  }
  return 1;
}

void CAN_BUS::can_parse(const CANFD_message_t &msg)
{
  std::string s = "";
  for (int i = 0; i < msg.len; i++)
    s += std::to_string(msg.buf[i]);
  Serial.print(s.c_str());
}