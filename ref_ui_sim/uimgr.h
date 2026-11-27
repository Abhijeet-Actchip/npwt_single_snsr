#ifndef __UI_MGR_H__
#define __UI_MGR_H__                 1

#include <stdint.h>
#include "uimgr_types.h"

int8_t UIMgrInit(const char *configFile);

int8_t UIMgrReadUIOutMsg(pub_sub_pkt_t **msg);
int8_t UIMgrSendUIInMsg(pub_sub_pkt_t *msg);
int8_t UIMgrSendUINotifyMsg(pub_sub_pkt_t *msg);

#endif /* __UI_MGR_H__ */