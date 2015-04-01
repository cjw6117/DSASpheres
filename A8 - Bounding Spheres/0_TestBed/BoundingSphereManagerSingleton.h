/*
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2015/03
*/
#ifndef __BOUNDINGSPHEREMANAGERSINGLETON_H_
#define __BOUNDINGSPHEREMANAGERSINGLETON_H_

#include "BoundingSphereClass.h"

//System Class
class BoundingSphereManagerSingleton
{
	int m_nSpheres; //number of elements in the list
	std::vector<BoundingSphereClass*> m_lSphere; //list of spheres
	std::vector<matrix4> m_lMatrix; //list of Global spaces of each sphere in the list
	std::vector<vector3> m_lColor; //list of colors of each sphere in the list
	static BoundingSphereManagerSingleton* m_pInstance; // Singleton pointer
	
public:
	/* Gets/Constructs the singleton pointer */
	static BoundingSphereManagerSingleton* GetInstance();
	/* Destroys the singleton */
	static void ReleaseInstance(void);

	/* Asks the singleton for the number of elements in the list */
	int GetSphereTotal(void);

	/*
	GenerateBoundingSphere
		Generates a bounding sphere from the specified instance name and adds it to the list
	*/
	void GenerateBoundingSphere(String a_sInstanceName);

	/*
	SetBoundingSphereSpace
		Sets the Matrix that represents the global position of the sphere specified by instance name
	*/
	void SetBoundingSphereSpace(matrix4 a_mModelToWorld, String a_sInstanceName);

	/*
	IdentifySphere
		Returns the index in which the specified sphere is stored in the list
	*/
	int IdentifySphere(String a_sInstanceName);

	/*
	AddSphereToRenderList
		Add the specified instance to the render list, ALL will add all of them at once
	*/
	void AddSphereToRenderList(String a_sInstanceName = "ALL");

	/*
	CalculateCollision
		Calculates the collision among all the spheres in the list
	*/
	void CalculateCollision(void);

private:
	/* Constructor */
	BoundingSphereManagerSingleton(void);
	/* Copy Constructor */
	BoundingSphereManagerSingleton(BoundingSphereManagerSingleton const& other);
	/* Copy Assignment Operator */
	BoundingSphereManagerSingleton& operator=(BoundingSphereManagerSingleton const& other);
	/* Destructor */
	~BoundingSphereManagerSingleton(void);

	/* Releases the objects memory */
	void Release(void);
	/* Initializates the objects fields */
	void Init(void);
};

#endif __BOUNDINGSPHEREMANAGERSINGLETON_H_