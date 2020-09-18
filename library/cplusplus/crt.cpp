/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
* Change Logs:
* Date           Author       Notes
* 2015-03-07     Bernard      Add copyright header.
*/

#include "cmsis_os.h"
#include <string.h>
#include "crt.h"

void *operator new(size_t size)
{
    void *vailed = pvPortMalloc(size);
    memset(vailed, 0, size);
    return vailed;
}

void *operator new[](size_t size)
{
    void *vailed = pvPortMalloc(size);
    memset(vailed, 0, size);
    return vailed;
}

void operator delete(void *ptr)
{
    vPortFree(ptr);
}

void operator delete[](void *ptr)
{
    vPortFree(ptr);
}
