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
			BoundingBoxClass* pBB = new BoundingBoxClass();
			pBB->SetColor(vector3(MEWHITE));
			//construct its information out of the instance name
			pBB->GenerateBoundingBox(a_sInstanceName);
			//Push the Box back into the list
			m_lBox.push_back(pBB);
			//Push a new matrix into the list
			m_lMatrix.push_back(matrix4(IDENTITY));
			//Specify the color the Box is going to have
			m_lColor.push_back(pBB->GetColor());
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

void BoundingBoxManagerSingleton::SetBoundingBoxVisibility(bool visible, String a_sInstanceName)
{
	int nBox = IdentifyBox(a_sInstanceName);
	//If the Box was found
	if(nBox != -1)
	{
		//set box visibility
		m_lBox[nBox]->SetVisible(visible);
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
			if(m_lBox[nBox]->GetVisible())
				m_lBox[nBox]->AddBoxToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
	else
	{
		int nBox = IdentifyBox(a_sInstanceName);
		if(nBox != -1)
		{
			if(m_lBox[nBox]->GetVisible())
				m_lBox[nBox]->AddBoxToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
}

void BoundingBoxManagerSingleton::CalculateCollision(void)
{
	//Create a placeholders
	std::vector<vector3> lCentroid;
	std::vector<vector3> lMins;
	std::vector<vector3> lMaxs;
	//for all boxes...
	for(int nBox = 0; nBox < m_nBoxs; nBox++)
	{
		//Make all the spheres white
		m_lColor[nBox] = vector3(1.0f);
		//Place all the centroids of spheres in global space
		lCentroid.push_back(static_cast<vector3>(m_lMatrix[nBox] * vector4(m_lBox[nBox]->GetCentroid(), 1.0f)));
		lMins.push_back(static_cast<vector3>(m_lMatrix[nBox] * vector4(m_lBox[nBox]->GetMin(), 1.0f)));
		lMaxs.push_back(static_cast<vector3>(m_lMatrix[nBox] * vector4(m_lBox[nBox]->GetMax(), 1.0f)));

	}

	for(int i = 0; i < m_nBoxs; i++)
	{
		for(int j = 0; j < m_nBoxs; j++)
		{
			if(i != j)
			{
				BoundingBoxClass* b1 = m_lBox[i];
				BoundingBoxClass* b2 = m_lBox[j];
				vector3 b1Centroid = lCentroid[i];
				vector3 b2Centroid = lCentroid[j];
				vector3 b1Min = lMins[i];
				vector3 b1Max = lMaxs[i];
				vector3 b2Min = lMins[j];
				vector3 b2Max = lMaxs[j];
				float CentDiffX = abs(b1Centroid.x - b2Centroid.x);
				float CentDiffY = abs(b1Centroid.y - b2Centroid.y);
				float CentDiffZ = abs(b1Centroid.z - b2Centroid.z);
				float b1HalfWidth = (b1Max.x - b1Min.x) / 2.0f;
				float b2HalfWidth = (b2Max.x - b2Min.x) / 2.0f;
				float b1HalfHeight = (b1Max.y - b1Min.y) / 2.0f;
				float b2HalfHeight = (b2Max.y - b2Min.y) / 2.0f;
				float b1HalfDepth = (b1Max.z - b1Min.z) / 2.0f;
				float b2HalfDepth = (b2Max.z - b2Min.z) / 2.0f;
				
				//If the distance between the center of both Boxess is less than the 
				//sum of their Half-Dimensions there is a collision
				if(CentDiffX < (b1HalfWidth + b2HalfWidth) &&
				   CentDiffY < (b1HalfHeight + b2HalfHeight) &&
				   CentDiffZ < (b1HalfDepth + b2HalfDepth))
					m_lColor[i] = m_lColor[j] = MERED; //We make the Boxs red
			}
		}
	}
	
}