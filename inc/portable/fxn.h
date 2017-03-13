/*
 * fxn.h
 *
 *  Created on: 19 Sep 2016
 *      Author: chiong
 *
 *  Wrapper for OS dependent functionalities.
 */

#ifndef INC_FXN_H_
#define INC_FXN_H_

#include <stdint.h>

/**
 * @brief Function declaration for delay function that is provided by the different OSes.
 */
typedef void (*fxnDelay_t) (uint32_t delay);

#endif /* INC_FXN_H_ */
