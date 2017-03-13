/*
 * assert.h
 *
 *  Created on: 8 Jul 2016
 *      Author: chiong
 */

#ifndef INC_ASSERT_H_
#define INC_ASSERT_H_

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(cond, msg)	\
	if (!(cond)) {	\
		fprintf(stderr, "%s:%s:%d > %s\n", __FILE__, __func__, __LINE__, msg);	\
		exit(EXIT_FAILURE);	\
	}

#endif /* INC_ASSERT_H_ */
