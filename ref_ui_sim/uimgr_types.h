#ifndef __UIMGR_TYPES_H__
#define __UIMGR_TYPES_H__                 1

#include "utils_c/pub_sub_types.h"
#include "utils_c/app_msg_q.h"
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct _uimgr_ctx_t
{
    /* Private variables */
    app_msg_q_t uiInMsgQueue;
    app_msg_q_t uiOutMsgQueue;
    app_msg_q_t uiNotifyMsgQueue;

    volatile int uiMgrActive;
    pthread_t uiMgrThreadHandle;
    pthread_t lvglThreadHandle;
} uimgr_ctx_t;

#endif /* __UIMGR_TYPES_H__ */