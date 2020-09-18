#include "ipc.h"
#include <cstring>

#include "ipchelper.h"

bool is_orb_multi(const int serial)
{
    return false;
}

int get_priority(const int instance)
{
    switch(instance)
    {
        case 1:
            return IPC_PRIO_LOW;
        case 0:
            return IPC_PRIO_HIGH;

        default:
            return -1;
    }
}

int get_orb_instance_according_to_priority(const int priority)
{
    switch(priority)
    {
        case IPC_PRIO_LOW:
            return 1;
        case IPC_PRIO_HIGH:
            return 0;
        default:
            return -1;
    }
}

