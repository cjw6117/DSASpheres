#pragma once
/*
Programmer: Jarrett Way (cjw6117@gmail.com)
Date: 2015/03
*/
#ifndef __BOUNDINGBOXCLASS_H_
#define __BOUNDINBOXCLASS_H_

#include "MyEngine.h"

//System Class
class BoundingBoxClass
{
	bool m_bInitialized; //Has the elements of this object been initialized before?
	bool m_bIsVisible;
	vector3 min;
	vector3 max;
	vector3 m_v3Centroid;//Center point of the model, the origin in GSpace in which the sphere is going to be located
	vector3 color;
	String m_sName;//Name of the BoundingSphere (to relate to the instance)

public:
	/* Constructor */
	BoundingBoxClass(void);
	/* Copy Constructor */
	BoundingBoxClass(BoundingBoxClass const& other);
	/* Copy Assignment Operator*/
	BoundingBoxClass& operator=(BoundingBoxClass const& other);
	/* Destructor */
	~BoundingBoxClass(void);

	/* Swaps the contents of the object with another object's content */
	void Swap(BoundingBoxClass& other);

	/*
	IsInitialized
		Returns information about whether or not the object has been initialized
	*/
	bool IsInitialized(void);

	/*
	GetCentroid
		Returns the origin of the Cartesian Coordinate System in which the sphere is drawn in GSpace
	*/
	vector3 GetCentroid(void);/*
	GetMin
		Returns the minimum values for each dimension
	*/
	vector3 GetMin(void);
	/*
	GetMax
		Returns the maximum values for each dimension
	*/
	vector3 GetMax(void);

	vector3 GetColor(void);

	void SetColor(vector3 color);

	bool GetVisible(void);

	void SetVisible(bool visible);

	/*
	GetName
		Returns the name of the sphere to associate it with an instance
	*/
	String GetName(void);

	/*
	ConstructSphere
		Creates the sphere in local space of the model specified in the instance name
	*/
	void GenerateBoundingBox(String a_sInstanceName);

	
	/*
	AddSphereToRenderList
		Adds the sphere to the render list specifying also if rendering the centroid is necessary
	*/
	void AddBoxToRenderList(matrix4 a_mModelToWorld, vector3 a_vColor, bool a_bRenderCentroid = false);

	
private:
	/* Releases the object from memory */
	void Release(void);

	/* Initialize the object's fields */
	void Init(void);
};


#endif //__BOUNDINGBOXCLASS_H__class BoundingBox


