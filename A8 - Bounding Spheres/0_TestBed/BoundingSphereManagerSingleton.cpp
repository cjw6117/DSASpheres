#include "BoundingSphereManagerSingleton.h"

//  BoundingSphereManagerSingleton
BoundingSphereManagerSingleton* BoundingSphereManagerSingleton::m_pInstance = nullptr;
void BoundingSphereManagerSingleton::Init(void)
{
	m_nSpheres = 0;
}
void BoundingSphereManagerSingleton::Release(void)
{
	//Clean the list of spheres
	for(int n = 0; n < m_nSpheres; n++)
	{
		//Make sure to release the memory of the pointers
		if(m_lSphere[n] != nullptr)
		{
			delete m_lSphere[n];
			m_lSphere[n] = nullptr;
		}
	}
	m_lSphere.clear();
	m_lMatrix.clear();
	m_lColor.clear();
	m_nSpheres = 0;
}
BoundingSphereManagerSingleton* BoundingSphereManagerSingleton::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new BoundingSphereManagerSingleton();
	}
	return m_pInstance;
}
void BoundingSphereManagerSingleton::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//The big 3
BoundingSphereManagerSingleton::BoundingSphereManagerSingleton(){Init();}
BoundingSphereManagerSingleton::BoundingSphereManagerSingleton(BoundingSphereManagerSingleton const& other){ }
BoundingSphereManagerSingleton& BoundingSphereManagerSingleton::operator=(BoundingSphereManagerSingleton const& other) { return *this; }
BoundingSphereManagerSingleton::~BoundingSphereManagerSingleton(){Release();};
//Accessors
int BoundingSphereManagerSingleton::GetSphereTotal(void){ return m_nSpheres; }

//--- Non Standard Singleton Methods
void BoundingSphereManagerSingleton::GenerateBoundingSphere(String a_sInstanceName)
{
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	//Verify the instance is loaded
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{//if it is check if the sphere has already been created
		if(IdentifySphere(a_sInstanceName) == -1)
		{
			//Create a new bounding sphere
			BoundingSphereClass* pBS = new BoundingSphereClass();
			//construct its information out of the instance name
			pBS->GenerateBoundingSphere(a_sInstanceName);
			//Push the sphere back into the list
			m_lSphere.push_back(pBS);
			//Push a new matrix into the list
			m_lMatrix.push_back(matrix4(IDENTITY));
			//Specify the color the sphere is going to have
			m_lColor.push_back(vector3(1.0f));
			//Increase the number of spheres
			m_nSpheres++;
		}
	}
}

void BoundingSphereManagerSingleton::SetBoundingSphereSpace(matrix4 a_mModelToWorld, String a_sInstanceName)
{
	int nSphere = IdentifySphere(a_sInstanceName);
	//If the sphere was found
	if(nSphere != -1)
	{
		//Set up the new matrix in the appropriate index
		m_lMatrix[nSphere] = a_mModelToWorld;
	}
}

int BoundingSphereManagerSingleton::IdentifySphere(String a_sInstanceName)
{
	//Go one by one for all the spheres in the list
	for(int nSphere = 0; nSphere < m_nSpheres; nSphere++)
	{
		//If the current sphere is the one we are looking for we return the index
		if(a_sInstanceName == m_lSphere[nSphere]->GetName())
			return nSphere;
	}
	return -1;//couldnt find it return with no index
}

void BoundingSphereManagerSingleton::AddSphereToRenderList(String a_sInstanceName)
{
	//If I need to render all
	if(a_sInstanceName == "ALL")
	{
		for(int nSphere = 0; nSphere < m_nSpheres; nSphere++)
		{
			m_lSphere[nSphere]->AddSphereToRenderList(m_lMatrix[nSphere], m_lColor[nSphere], true);
		}
	}
	else
	{
		int nSphere = IdentifySphere(a_sInstanceName);
		if(nSphere != -1)
		{
			m_lSphere[nSphere]->AddSphereToRenderList(m_lMatrix[nSphere], m_lColor[nSphere], true);
		}
	}
}

void BoundingSphereManagerSingleton::CalculateCollision(void)
{
	//Create a placeholder for all center points
	std::vector<vector3> lCentroid;
	//for all spheres...
	for(int nSphere = 0; nSphere < m_nSpheres; nSphere++)
	{
		//Make all the spheres white
		m_lColor[nSphere] = vector3(1.0f);
		//Place all the centroids of spheres in global space
		lCentroid.push_back(static_cast<vector3>(m_lMatrix[nSphere] * vector4(m_lSphere[nSphere]->GetCentroid(), 1.0f)));
	}

	//now the actual check for all spheres among all spheres (so... one by one), this is not the most optimal way, there is a more clever one
	for(int i = 0; i < m_nSpheres; i++)
	{
		for(int j = 0; j < m_nSpheres; j++)
		{
			if(i != j)
			{
				//If the distance between the center of both spheres is less than the sum of their radius there is a collision
				if(glm::distance(lCentroid[i], lCentroid[j]) < (m_lSphere[i]->GetRadius() + m_lSphere[j]->GetRadius()))
					m_lColor[i] = m_lColor[j] = MERED; //We make the spheres red
			}
		}
	}
	

	////This way is more optimal, just half the checks are needed
	//for(int i = 0; i < m_nSpheres - 1; i++)
	//{
	//	for(int j = i + 1; j < m_nSpheres; j++)
	//	{
	//		//If the distance between the center of both spheres is less than the sum of their radius there is a collision
	//		if(glm::distance(lCentroid[i], lCentroid[j]) < (m_lSphere[i]->GetRadius() + m_lSphere[j]->GetRadius()))
	//			m_lColor[i] = m_lColor[j] = MERED; //We make the spheres red
	//	}
	//}
}