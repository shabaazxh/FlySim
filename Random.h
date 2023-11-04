///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	Random.h
//	------------------------
//	
//	Some random utilities
//	
///////////////////////////////////////////////////

#ifndef __RANDOM_H
#define __RANDOM_H

#include "Cartesian3.h"
// generates a single random value in a given range
float RandomRange(float minimum, float maximum);

// generates a random vector with components in a given range
// NO Monte Carlo at this stage - that is taken care of in the calling function
Cartesian3 RandomVector(float minimum, float maximum);

// generates a random vector with Monte Carlo simulation
Cartesian3 RandomUnitVectorInUpwardsCone(float minimumAngle, float minimumLength, float maximumLength);
#endif
