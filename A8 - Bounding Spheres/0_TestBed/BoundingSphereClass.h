/*
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2015/03
*/
#ifndef __BOUNDINGSPHERECLASS_H_
#define __BOUNDINGSPHERECLASS_H_

#include "MyEngine.h"

//System Class
class BoundingSphereClass
{
	bool m_bInitialized; //Has the elements of this object been initialized before?
	float m_fRadius;//Radius of the collision sphere
	vector3 m_v3Centroid;//Center point of the model, the origin in GSpace in which the sphere is going to be located
	String m_sName;//Name of the BoundingSphere (to relate to the instance)

public:
	/* Constructor */
	BoundingSphereClass(void);
	/* Copy Constructor */
	BoundingSphereClass(BoundingSphereClass const& other);
	/* Copy Assignment Operator*/
	BoundingSphereClass& operator=(BoundingSphereClass const& other);
	/* Destructor */
	~BoundingSphereClass(void);

	/* Swaps the contents of the object with another object's content */
	void Swap(BoundingSphereClass& other);

	/*
	IsInitialized
		Returns information about whether or not the object has been initialized
	*/
	bool IsInitialized(void);

	/*
	GetRadius
		Returns the radius of the sphere
	*/
	float GetRadius(void);

	/*
	GetCentroid
		Returns the origin of the Cartesian Coordinate System in which the sphere is drawn in GSpace
	*/
	vector3 GetCentroid(void);

	/*
	GetName
		Returns the name of the sphere to associate it with an instance
	*/
	String GetName(void);

	/*
	ConstructSphere
		Creates the sphere in local space of the model specified in the instance name
	*/
	void GenerateBoundingSphere(String a_sInstanceName);

	
	/*
	AddSphereToRenderList
		Adds the sphere to the render list specifying also if rendering the centroid is necessary
	*/
	void AddSphereToRenderList(matrix4 a_mModelToWorld, vector3 a_vColor, bool a_bRenderCentroid = false);

	
private:
	/* Releases the object from memory */
	void Release(void);

	/* Initialize the object's fields */
	void Init(void);
};


#endif //__BOUNDINGSPHERECLASS_H__