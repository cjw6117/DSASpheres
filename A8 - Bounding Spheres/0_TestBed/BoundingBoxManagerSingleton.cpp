#include "BoundingBoxManagerSingleton.h"


//  BoundingBoxManagerSingleton
BoundingBoxManagerSingleton* BoundingBoxManagerSingleton::m_pInstance = nullptr;
void BoundingBoxManagerSingleton::Init(void)
{
	m_nBoxs = 0;
}
void BoundingBoxManagerSingleton::Release(void)
{
	//Clean the list of Boxs
	for(int n = 0; n < m_nBoxs; n++)
	{
		//Make sure to release the memory of the pointers
		if(m_lBox[n] != nullptr)
		{
			delete m_lBox[n];
			m_lBox[n] = nullptr;
		}
	}
	m_lBox.clear();
	m_lMatrix.clear();
	m_lColor.clear();
	m_nBoxs = 0;
}
BoundingBoxManagerSingleton* BoundingBoxManagerSingleton::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new BoundingBoxManagerSingleton();
	}
	return m_pInstance;
}
void BoundingBoxManagerSingleton::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//The big 3
BoundingBoxManagerSingleton::BoundingBoxManagerSingleton(){Init();}
BoundingBoxManagerSingleton::BoundingBoxManagerSingleton(BoundingBoxManagerSingleton const& other){ }
BoundingBoxManagerSingleton& BoundingBoxManagerSingleton::operator=(BoundingBoxManagerSingleton const& other) { return *this; }
BoundingBoxManagerSingleton::~BoundingBoxManagerSingleton(){Release();};
//Accessors
int BoundingBoxManagerSingleton::GetBoxTotal(void){ return m_nBoxs; }

//--- Non Standard Singleton Methods
void BoundingBoxManagerSingleton::GenerateBoundingBox(String a_sInstanceName)
{
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	//Verify the instance is loaded
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{//if it is check if the Box has already been created
		if(IdentifyBox(a_sInstanceName) == -1)
		{
			//Create a new bounding Box
			BoundingBoxClass* pBS = new BoundingBoxClass();
			//construct its information out of the instance name
			pBS->GenerateBoundingBox(a_sInstanceName);
			//Push the Box back into the list
			m_lBox.push_back(pBS);
			//Push a new matrix into the list
			m_lMatrix.push_back(matrix4(IDENTITY));
			//Specify the color the Box is going to have
			m_lColor.push_back(vector3(1.0f));
			//Increase the number of Boxs
			m_nBoxs++;
		}
	}
}

void BoundingBoxManagerSingleton::SetBoundingBoxSpace(matrix4 a_mModelToWorld, String a_sInstanceName)
{
	int nBox = IdentifyBox(a_sInstanceName);
	//If the Box was found
	if(nBox != -1)
	{
		//Set up the new matrix in the appropriate index
		m_lMatrix[nBox] = a_mModelToWorld;
	}
}

int BoundingBoxManagerSingleton::IdentifyBox(String a_sInstanceName)
{
	//Go one by one for all the Boxs in the list
	for(int nBox = 0; nBox < m_nBoxs; nBox++)
	{
		//If the current Box is the one we are looking for we return the index
		if(a_sInstanceName == m_lBox[nBox]->GetName())
			return nBox;
	}
	return -1;//couldnt find it return with no index
}

void BoundingBoxManagerSingleton::AddBoxToRenderList(String a_sInstanceName)
{
	//If I need to render all
	if(a_sInstanceName == "ALL")
	{
		for(int nBox = 0; nBox < m_nBoxs; nBox++)
		{
			m_lBox[nBox]->AddBoxToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
	else
	{
		int nBox = IdentifyBox(a_sInstanceName);
		if(nBox != -1)
		{
			m_lBox[nBox]->AddBoxToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
}

void BoundingBoxManagerSingleton::CalculateCollision(void)
{
	//Create a placeholder for all center points
	std::vector<vector3> lCentroid;
	//for all spheres...
	for(int nBox = 0; nBox < m_nBoxs; nBox++)
	{
		//Make all the spheres white
		m_lColor[nBox] = vector3(1.0f);
		//Place all the centroids of spheres in global space
		lCentroid.push_back(static_cast<vector3>(m_lMatrix[nBox] * vector4(m_lBox[nBox]->GetCentroid(), 1.0f)));
	}

	for(int i = 0; i < m_nBoxs; i++)
	{
		for(int j = 0; j < m_nBoxs; j++)
		{
			if(i != j)
			{
				BoundingBoxClass* b1 = m_lBox[i];
				BoundingBoxClass* b2 = m_lBox[j];
				vector3 b1Centroid = b1->GetCentroid();
				vector3 b2Centroid = b2->GetCentroid();
				float CentDiffX = abs(b1Centroid.x - b2Centroid.x);
				float CentDiffY = abs(b1Centroid.y - b1Centroid.y);
				float CentDiffZ = abs(b1Centroid.z - b2Centroid.z);
				float b1HalfWidth = (b1->GetMax().x - b1->GetMin().x) / 2.0f;
				float b2HalfWidth = (b2->GetMax().x - b2->GetMin().x) / 2.0f;
				float b1HalfHeight = (b1->GetMax().y - b1->GetMin().y) / 2.0f;
				float b2HalfHeight = (b2->GetMax().y - b2->GetMin().y) / 2.0f;
				float b1HalfDepth = (b1->GetMax().z - b1->GetMin().z) / 2.0f;
				float b2HalfDepth = (b2->GetMax().z - b2->GetMin().z) / 2.0f;
				/*if ( CentDiffX < (b1HalfWidth + b2HalfWidth) )
					m_lColor[i] = m_lColor[j] = MERED;*/

				/*if(b1->GetMax().x > b2->GetMin().x && 
					b1->GetMin().x < b2->GetMax().x &&
					b1->GetMax().y > b2->GetMin().y &&
					b1->GetMin().y < b2->GetMax().y &&
					b1->GetMax().z > b2->GetMin().z &&
					b1->GetMin().z < b2->GetMax().z)
					m_lColor[i] = m_lColor[j] = MERED;
*/
				//If the distance between the center of both Boxs is less than the sum of their radius there is a collision
				if(glm::distance(lCentroid[i], lCentroid[j]) < (b1HalfWidth + b2HalfWidth))
					m_lColor[i] = m_lColor[j] = MERED; //We make the Boxs red
				if(glm::distance(lCentroid[i], lCentroid[j]) < (b1HalfHeight + b2HalfHeight))
					m_lColor[i] = m_lColor[j] = MERED; //We make the Boxs red
				if(glm::distance(lCentroid[i], lCentroid[j]) < (b1HalfDepth + b2HalfDepth))
					m_lColor[i] = m_lColor[j] = MERED; //We make the Boxs red
			}
		}
	}

	////Create a placeholder for all center points
	//std::vector<vector3> lCentroid;
	////for all Boxs...
	//for(int nBox = 0; nBox < m_nBoxs; nBox++)
	//{
	//	//Make all the Boxs white
	//	m_lColor[nBox] = vector3(1.0f);
	//	//Place all the centroids of Boxs in global space
	//	lCentroid.push_back(static_cast<vector3>(m_lMatrix[nBox] * vector4(m_lBox[nBox]->GetCentroid(), 1.0f)));
	//}

	////now the actual check for all Boxs among all Boxs (so... one by one), this is not the most optimal way, there is a more clever one
	//for(int i = 0; i < m_nBoxs; i++)
	//{
	//	for(int j = 0; j < m_nBoxs; j++)
	//	{
	//		if(i != j)
	//		{
	//			//If the distance between the center of both Boxs is less than the sum of their radius there is a collision
	//			//if(glm::distance(lCentroid[i], lCentroid[j]) < (m_lBox[i]->GetRadius() + m_lBox[j]->GetRadius()))
	//			//	m_lColor[i] = m_lColor[j] = MERED; //We make the Boxs red
	//		}
	//	}
	//}
	
}