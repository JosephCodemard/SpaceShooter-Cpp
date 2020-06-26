#pragma once
#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <iostream>

namespace cfn {
    template <class T, class U> 
	bool OnCollision(T* _class1, U* _class2) {
		bool status = false;

		if (_class1 != NULL && _class2 != NULL) {
			if (_class1->xPos < _class2->xPos + _class2->width && _class1->xPos + _class1->width> _class2->xPos && _class1->yPos < _class2->yPos + _class2->height && _class1->yPos + _class1->height >_class2->yPos)
				status = true;
		}
		return status;
	}
}

#endif // COMMON_FUNCTIONS_H