///////////////////////////////////////////////////
//
//	Hamish Carr
//	October, 2023
//
//	------------------------
//	Terrain.h
//	------------------------
//	
//	A subclass of HomogeneousFaceSurface for terrain
//	
///////////////////////////////////////////////////

#ifndef _TERRAIN_H
#define _TERRAIN_H

#include <vector>

#include "HomogeneousFaceSurface.h"

struct AABB
{
	Cartesian3 min;
	Cartesian3 max;
};

class Terrain : public HomogeneousFaceSurface
	{ // class Terrain
	public:
	// array to store the terrain data
	std::vector<std::vector<float>> heightValues;
	
	// keep track of the xy scale that we are told about
	float xyScale;

	// constructor will initialise to safe values
	Terrain();

	AABB CreateAABB()
	{
		AABB box;

		box.min.x = box.min.y = box.min.z = 0.0f;
		box.max.x = heightValues[0].size() - 1;
		box.max.y = heightValues[0][0];
		box.max.z = heightValues.size() - 1;

		for(const auto& row : heightValues)
		{
			for(const auto& height : row)
			{
				box.min.y = std::min(box.min.y, height);
				box.max.y = std::max(box.max.y, height);
			}
		}

		return box;
	}
	void EditMesh(const Cartesian3& hitpoint, float radius, const columnMajorMatrix& matrix)
	{
		
		for(auto& vertex: vertices)
		{    
			float x = vertex.x - hitpoint.x;
			float y = vertex.y - hitpoint.z;
			float dist = sqrt(x*x + y*y);
			float force = 8.0f;
			if(dist <= radius * force)
			{
				std::cout << "Doing this " << std::endl;
				float a = ((radius - dist) / radius) * force;
				vertex.z += a * 1.0f;
			}
		}	
	}
	// read routine returns true on success, failure otherwise
	// xyScale gives the scale factor to use in the x-y directions
	bool ReadFileTerrainData(const char *fileName, float XYScale);
	
	// A function to find the height at a known (x,y) coordinate
	float getHeight(float x, float y);

	int m_width = 0;
	int m_height = 0;
	
	}; // class Terrain

#endif
