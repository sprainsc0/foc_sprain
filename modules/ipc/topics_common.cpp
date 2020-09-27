#include "ipc.h"
#include "topics/actuator_notify.h"
#include "topics/parameter_update.h"
#include "topics/foc_command.h"
#include "topics/telemetry_status.h"
#include "topics/encoder.h"
#include "topics/foc_status.h"
#include "topics/obs_status.h"
#include "topics/foc_target.h"
#include "topics/hfi_status.h"
#include "topics/uavcan_parameter_request.h"
#include "topics/uavcan_parameter_value.h"
#include "topics/uavcan_command_request.h"
#include "topics/uavcan_command_response.h"
#include "topics/calibrate_status.h"

//         name                       uORB struct                      serial id   buffer   buffer
IPC_DEFINE(actuator_notify,           struct actuator_notify_s,           0,        false,    0);
IPC_DEFINE(parameter_update,          struct parameter_update_s,          1,        false,    0);
IPC_DEFINE(foc_command,               struct foc_command_s,               2,        false,    0);
IPC_DEFINE(telemetry_status,          struct telemetry_status_s,          3,        false,    0);
IPC_DEFINE(encoder,                   struct encoder_s,                   4,        false,    0);
IPC_DEFINE(foc_status,                struct foc_status_s,                5,        false,    0);
IPC_DEFINE(calibrate_status,          struct calibrate_status_s,          6,        false,    0);
IPC_DEFINE(foc_target,                struct foc_target_s,                11,       false,    0);
IPC_DEFINE(obs_status,                struct obs_status_s,                12,       false,    0);
IPC_DEFINE(hfi_status,                struct hfi_status_s,                13,       false,    0);
// queue buffer for can
IPC_DEFINE(uavcan_parameter_request,  struct uavcan_parameter_request_s,  7,        true,     3);
IPC_DEFINE(uavcan_parameter_value,    struct uavcan_parameter_value_s,    8,        true,     3);
IPC_DEFINE(uavcan_command_request,    struct uavcan_command_request_s,    9,        true,     3);
IPC_DEFINE(uavcan_command_response,   struct uavcan_command_response_s,   10,       true,     3);