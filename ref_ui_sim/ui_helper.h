#ifndef __UI_HELPER_H__
#define __UI_HELPER_H__                 1

#include <stdint.h>
#include "uimgr_types.h"

int8_t UIHelperInitLvgl(uimgr_ctx_t *ctx);
int8_t UIHelperProcessNotifyMsgs(pub_sub_pkt_t *msg);

#endif /* __UI_HELPER_H__ */