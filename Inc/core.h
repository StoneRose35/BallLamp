/*
 * core.h
 *
 *  Created on: Oct 26, 2021
 *      Author: philipp
 */

#ifndef CORE_H_
#define CORE_H_
#include "types.h"

#define CPACR ((volatile uint32_t*)0xE000ED88)

void enableFpu();

#endif /* CORE_H_ */
