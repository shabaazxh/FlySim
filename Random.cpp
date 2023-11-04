///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	Random.cpp
//	------------------------
//	
//	Some random utilities
//	
///////////////////////////////////////////////////

#include <stdlib.h>
#include <math.h>
#include "Random.h"

// generates a single random value in a given range
float RandomRange(float minimum, float maximum)
	{ // RandomRange()
	// compute the total range
	float range = maximum - minimum;
	// generate a random number from 0 to 1
	float randomNumber = (float) random() / (float) RAND_MAX;
	// multiply by the range
	randomNumber *= range;
	// add to the minimum and return
	return randomNumber + minimum;	
	} // RandomRange()

// generates a random vector with components in a given range
// NO Monte Carlo at this stage - that is taken care of in the calling function
Cartesian3 RandomVector(float minimum, float maximum)
	{ // RandomVector()
	// the result
	Cartesian3 result;
	// generate random values separately for each component
	result.x = RandomRange(minimum, maximum);
	result.y = RandomRange(minimum, maximum);
	result.z = RandomRange(minimum, maximum);
	// and return it
	return result;
	} // RandomVector()

// generates a random vector with Monte Carlo simulation
Cartesian3 RandomUnitVectorInUpwardsCone(float minimumAngle, float minimumLength, float maximumLength)
	{ // RandomUnitVectorInUpwardsCone()
	// the result
	Cartesian3 result;
	
	// use the minimum vertical angle to compute a cosine DOWNWARDS from azimuth
	// i.e. use the sine instead. ;-)
	float minimumCosineValue = sin(minimumAngle);
	
	// loop until we get a valid result
	while (true)
		{ // loop until good vector
		// get a random vector: note that we *could* have components less than
		// minimum length, so we play safe and use 0.0 instead.  But no component
		// may be greater than maximumLength. . . 
		result = RandomVector(minimumLength, maximumLength); 
	
		// now get the length of the vector
		float resultLength = result.length();
		
		// and perform some tests, discarding vectors with bad lengths
		if (resultLength < minimumLength)
			continue;
		if (resultLength > maximumLength)
			continue;
			
		// take dot product and compare with minimum angle
		// if it's a satisfactory angle, return immediately
		if (result.dot(Cartesian3(0.0, 1.0, 0.0)) > minimumCosineValue * resultLength)
			break;

		} // loop until good vector


		float speed = RandomRange(60.0f, 300.0f);

		result = {result.x * speed, result.y * speed, result.z * speed};

		return result;
	// fall through to keep compiler happy	
	// return Cartesian3(0.0, 0.0, 0.0);
	} // RandomUnitVectorInUpwardsCone()
