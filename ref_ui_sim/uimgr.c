#include "uimgr.h"
#include "ui_helper.h"
#include "utils_c/log_pub.h"
#include "utils_c/app_err.h"
#include "utils_c/zmq_restapi_client.h"
#include <unistd.h>
#include <assert.h>

#define TAG                             "UI-MGR"
#define UI_MSG_QUEUE_SIZE               32

static uimgr_ctx_t gUIMgrCtx = {0};

static void ProcessUIInMsgs(uimgr_ctx_t *ctx)
{
    pub_sub_pkt_t *msg = NULL;
    int8_t retVal;

    retVal = AppMsgQReceive(&ctx->uiInMsgQueue,
                (void **) &msg, 0);
    if(retVal == APP_ERR_SUCCESS)
    {
        if(msg != NULL)
        {
            LogPubPrintF(TAG, "Resp: %s", msg->data);
            ZMQRestapiClientDispatchRespCb(msg);
            FreePubSubPkt(msg);
        }
    }
}

static void ProcessUINotifyMsgs(uimgr_ctx_t *ctx)
{
    pub_sub_pkt_t *msg = NULL;
    int8_t retVal;
    static int notifyCount = 0;

    retVal = AppMsgQReceive(&ctx->uiNotifyMsgQueue,
                (void **) &msg, 0);
    if(retVal == APP_ERR_SUCCESS)
    {
        if(msg != NULL)
        {
            notifyCount++;
            /* Print packets at a slower interval as it is flooding the logs. */
            if((notifyCount % 10) == 0)
            {
                LogPubPrintF(TAG, "Notify: %s", msg->data);
                notifyCount = 0;
            }
            UIHelperProcessNotifyMsgs(msg);
            FreePubSubPkt(msg);
        }
    }
}

static void *UIMgrThread(void *arg)
{
    int8_t ret;

    ret = UIHelperInitLvgl(&gUIMgrCtx);
    assert(ret == APP_ERR_SUCCESS);

    while(gUIMgrCtx.uiMgrActive)
    {
        ProcessUIInMsgs(&gUIMgrCtx);
        ProcessUINotifyMsgs(&gUIMgrCtx);
        ZMQRestapiClientCheckTimeouts();

        usleep(10000);
    }
    return NULL;
}

int8_t UIMgrInit(const char *configFile)
{
    int8_t ret;

    /* TODO: Parse configuration if any. */

    ret = AppMsgQInit(&gUIMgrCtx.uiInMsgQueue, UI_MSG_QUEUE_SIZE);
    if(ret != APP_ERR_SUCCESS)
    {
        LogPubPrintF(TAG, "Err: UIMgrInit: Failed to initialize UI-In message queue");
        return ret;
    }

    ret = AppMsgQInit(&gUIMgrCtx.uiOutMsgQueue, UI_MSG_QUEUE_SIZE);
    if(ret != APP_ERR_SUCCESS)
    {
        LogPubPrintF(TAG, "Err: UIMgrInit: Failed to initialize UI-Out message queue");
        return ret;
    }

    ret = AppMsgQInit(&gUIMgrCtx.uiNotifyMsgQueue, UI_MSG_QUEUE_SIZE);
    if(ret != APP_ERR_SUCCESS)
    {
        LogPubPrintF(TAG, "Err: UIMgrInit: Failed to initialize UI-Notify message queue");
        return ret;
    }

    gUIMgrCtx.uiMgrActive = 1;
    ret = pthread_create(&gUIMgrCtx.uiMgrThreadHandle, NULL, UIMgrThread, NULL);
    if(ret != APP_ERR_SUCCESS)
    {
        LogPubPrintF(TAG, "Err: UIMgrInit: Failed to create UI manager thread");
        gUIMgrCtx.uiMgrActive = 0;
        return ret;
    }

    return APP_ERR_SUCCESS;
}

int8_t UIMgrReadUIOutMsg(pub_sub_pkt_t **msg)
{
    if(msg == NULL)
    {
        LogPubPrintF(TAG, "Err: UIMgrReadUIOutMsg: Invalid parameters");
        return APP_ERR_INVALID_PARAMS;
    }

    return AppMsgQReceive(&gUIMgrCtx.uiOutMsgQueue, (void **) msg, 1);
}

int8_t UIMgrSendUIInMsg(pub_sub_pkt_t *msg)
{
    if(msg == NULL)
    {
        LogPubPrintF(TAG, "Err: UIMgrSendUIInMsg: Invalid parameters");
        return APP_ERR_INVALID_PARAMS;
    }

    return AppMsgQSend(&gUIMgrCtx.uiInMsgQueue, msg, 1);
}

int8_t UIMgrSendUINotifyMsg(pub_sub_pkt_t *msg)
{
    if(msg == NULL)
    {
        LogPubPrintF(TAG, "Err: UIMgrSendUINotifyMsg: Invalid parameters");
        return APP_ERR_INVALID_PARAMS;
    }

    return AppMsgQSend(&gUIMgrCtx.uiNotifyMsgQueue, msg, 1);
}