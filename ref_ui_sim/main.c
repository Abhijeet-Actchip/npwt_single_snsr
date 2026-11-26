#include "uimgr.h"
#include "utils_c/log_pub.h"
#include "utils_c/config_mgr.h"
#include "utils_c/zmq_pub_sub.h"
#include "utils_c/app_err.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#define TAG                             "UI-MGR"

/*!
 * \brief Configuration structure for uimgr initialization
 */
typedef struct _uimgr_config_t
{
    zmq_cnf_t uiChannels;           /*!< UI IN/OUT channels */
    zmq_cnf_t uiNotifyChannels;     /*!< UI Notify channels. */
} uimgr_config_t;
 
static uimgr_config_t config;
static zmq_channels_t channels;

static void UIInSubCb(const uint8_t *data, size_t len)
{
    assert(data && len > 0);

    pub_sub_pkt_t *ptrMsg = AllocPubSubPkt(len);
    if(ptrMsg == NULL)
    {
        LogPubPrintF(TAG, "Err: UIInSubCb: Failed to allocate memory");
        return;
    }

    memcpy(ptrMsg->data, data, len);
    ptrMsg->dataLen = len;

    int8_t ret;
    /* Pass UI-In messages to UI Manager. */
    ret = UIMgrSendUIInMsg(ptrMsg);
    if(ret != APP_ERR_SUCCESS)
    {
        LogPubPrintF(TAG, "Err: UIInSubCb: Failed to send message to UI Manager");
        FreePubSubPkt(ptrMsg);
    }
}

static void UINotifySubCb(const uint8_t *data, size_t len)
{
    assert(data && len > 0);

    pub_sub_pkt_t *ptrMsg = AllocPubSubPkt(len);
    if(ptrMsg == NULL)
    {
        LogPubPrintF(TAG, "Err: UINotifySubCb: Failed to allocate memory");
        return;
    }

    memcpy(ptrMsg->data, data, len);
    ptrMsg->dataLen = len;

    int8_t ret;
    /* Pass UI-Notify messages to UI Manager. */
    ret = UIMgrSendUINotifyMsg(ptrMsg);
    if(ret != APP_ERR_SUCCESS)
    {
        LogPubPrintF(TAG, "Err: UINotifySubCb: Failed to send message to UI Manager");
        FreePubSubPkt(ptrMsg);
    }
}

static void ProcessUIOutMsgs(void)
{
    pub_sub_pkt_t *ptrMsg = NULL;

    if(UIMgrReadUIOutMsg(&ptrMsg) == APP_ERR_SUCCESS)
    {
        if(ptrMsg != NULL)
        {
            /* Publish UI-Out messages. */
            ZmqPublish(&config.uiChannels, ptrMsg->data, ptrMsg->dataLen);
            FreePubSubPkt(ptrMsg);
        }
        else
        {
            LogPubPrintF(TAG, "Err: ProcessUIOutMsgs: NULL message received");
        }
    }
}

int main(int argc, char *argv[])
{
    int8_t ret;
    assert(argc > 1);
    
    ret = ConfigMgrParseZmqChannels(argv[1], &channels);
    assert(ret == APP_ERR_SUCCESS);

    /* Initialize log publisher */
    ret = LogPubInit(channels.chanLogPub);
    assert(ret == APP_ERR_SUCCESS);

    /* Initialize csv based app log publisher */
    ret = AppLogPubInit(channels.chanAppLogInPub);
    assert(ret == APP_ERR_SUCCESS);

    /* Wait for log publisher to initialize. */
    sleep(1);

    /* Initialize App Manager */
    ret = UIMgrInit(argv[1]);
    assert(ret == APP_ERR_SUCCESS);

    /* Initialize ZeroMQ (UI IN/OUT channels) */
    config.uiChannels.pub_port = channels.chanUiOutPub;
    config.uiChannels.sub_port = channels.chanUiInSub;
    config.uiChannels.subCb = UIInSubCb;
    ret = ZmqPubSubInit(&config.uiChannels);
    assert(ret == APP_ERR_SUCCESS);

    /* Initialize ZeroMQ (UI Notify channels) */
    config.uiNotifyChannels.isSubOnly = true;
    config.uiNotifyChannels.sub_port = channels.chanUiNotifySub;
    config.uiNotifyChannels.subCb = UINotifySubCb;
    ret = ZmqPubSubInit(&config.uiNotifyChannels);
    assert(ret == APP_ERR_SUCCESS);

    LogPubPrintF(TAG, "Initialized successfully");

    while (1)
    {
        /* Publish UI-Out messages. */
        ProcessUIOutMsgs();
        usleep(10000);
    }
    
    return 0;
}