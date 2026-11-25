/*
 * thpy_interface.h
 *
 *  Created on: Mar 17, 2025
 *      Author: Mahesh Murty
 */

#ifndef __THPY_BASE_H__
#define __THPY_BASE_H__				1

#include <stdint.h>
#include "thpy_common.h"

int8_t ThpyInit(thpy_class_t *thpyObj);
bool IsThpyInit(thpy_class_t *thpyObj);

#endif /* __THPY_BASE_H__ */
