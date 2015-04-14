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
void BoundingBoxManagerSingleton::GenerateBoundingBox(matrix4 a_mModelToWorld, String a_sInstanceName)
{
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	//Verify the instance is loaded
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{//if it is check if the Box has already been created
		int nBox = IdentifyBox(a_sInstanceName);
		if(nBox == -1)
		{
			//Create a new bounding Box
			BoundingBoxClass* pBB = new BoundingBoxClass();
			//construct its information out of the instance name
			pBB->GenerateOrientedBoundingBox(a_sInstanceName);
			//Push the Box back into the list
			m_lBox.push_back(pBB);
			//Push a new matrix into the list
			m_lMatrix.push_back(matrix4(IDENTITY));
			//Specify the color the Box is going to have
			m_lColor.push_back(vector3(1.0f));
			//Increase the number of Boxes
			m_nBoxs++;
		}
		else //If the box has already been created you will need to check its global orientation
		{
			m_lBox[nBox]->GenerateAxisAlignedBoundingBox(a_mModelToWorld);
		}
		nBox = IdentifyBox(a_sInstanceName);
		m_lMatrix[nBox] = a_mModelToWorld;
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
	return -1;//couldn't find it return with no index
}

void BoundingBoxManagerSingleton::AddBoxToRenderList(String a_sInstanceName)
{
	//If I need to render all
	if(a_sInstanceName == "ALL")
	{
		for(int nBox = 0; nBox < m_nBoxs; nBox++)
		{
			m_lBox[nBox]->AddAABBToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
	else
	{
		int nBox = IdentifyBox(a_sInstanceName);
		if(nBox != -1)
		{
			m_lBox[nBox]->AddAABBToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
}

void BoundingBoxManagerSingleton::CalculateCollision(void)
{
	//Create a placeholder for all center points
	std::vector<vector3> lCentroid;
	//for all Boxs...
	for(int nBox = 0; nBox < m_nBoxs; nBox++)
	{
		//Make all the Boxs white
		m_lColor[nBox] = vector3(1.0f);
		//Place all the centroids of Boxs in global space
		lCentroid.push_back(static_cast<vector3>(m_lMatrix[nBox] * vector4(m_lBox[nBox]->GetCentroid(), 1.0f)));
	}

	//Now the actual check
	for(int i = 0; i < m_nBoxs - 1; i++)
	{
		for(int j = i + 1; j < m_nBoxs; j++)
		{
			//If the distance between the center of both Boxs is less than the sum of their radius there is a collision
			//For this check we will assume they will be colliding unless they are not in the same space in X, Y or Z
			//so we place them in global positions
			vector3 v1Min = m_lBox[i]->GetMinimumAABB();
			vector3 v1Max = m_lBox[i]->GetMaximumAABB();

			vector3 v2Min = m_lBox[j]->GetMinimumAABB();
			vector3 v2Max = m_lBox[j]->GetMaximumAABB();

			bool bColliding = true;
			if(v1Max.x < v2Min.x || v1Min.x > v2Max.x)
				bColliding = false;
			else if(v1Max.y < v2Min.y || v1Min.y > v2Max.y)
				bColliding = false;
			else if(v1Max.z < v2Min.z || v1Min.z > v2Max.z)
				bColliding = false;

			if(bColliding) {
				
				//Now do a OBB check with SAT
				if(OBB_SAT_Check(i, j)) {
					m_lColor[i] = m_lColor[j] = MERED; //We make the Boxes red
				}
			}
		}
	}
}

bool BoundingBoxManagerSingleton::OBB_SAT_Check(int indexA, int indexB)
{
	BoundingBoxClass* A = m_lBox[indexA];
	BoundingBoxClass* B = m_lBox[indexB];
	vector4 aMin = m_lMatrix[indexA] * vector4(A->GetMinimumOBB(), 1.0f);
	vector4 aMax = m_lMatrix[indexA] * vector4(A->GetMaximumOBB(), 1.0f);
	vector4 aCent = m_lMatrix[indexA] * vector4(A->GetCentroid(), 1.0f);

	vector4 bMin = m_lMatrix[indexB] * vector4(B->GetMinimumOBB(), 1.0f);
	vector4 bMax = m_lMatrix[indexB] * vector4(B->GetMaximumOBB(), 1.0f);
	vector4 bCent = m_lMatrix[indexB] * vector4(B->GetCentroid(), 1.0f);

	float radiusA;
	float radiusB;
	glm::mat3x3 R, AbsR;

	float aR[3];
	aR[0] = abs(aMax.x - aCent.x);
	aR[1] = abs(aMax.y - aCent.y);
	aR[2] = abs(aMax.z - aCent.z);

	float bR[3];
	bR[0] = abs(bMax.x - bCent.x);
	bR[1] = abs(bMax.y - bCent.y);
	bR[2] = abs(bMax.z - bCent.z);

	//Calculate rotation matrix representing b in a's coordinate frame
	/*for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			R[i][j] = glm::dot(m_lMatrix[indexA][i], m_lMatrix[indexB][j]);
		}
	}*/
	R = static_cast<glm::mat3x3>(m_lMatrix[indexA] * (m_lMatrix[indexB] / glm::inverse(m_lMatrix[indexB])));

	//Compute translation vector t

	vector3 t = static_cast<vector3>(bCent - aCent);
	//Brind translation into a's coordinate frame
	float tx = glm::dot(vector4(t, 1.0f), m_lMatrix[indexA][0]);
	float ty = glm::dot(vector4(t, 1.0f), m_lMatrix[indexA][2]);
	float tz = glm::dot(vector4(t, 1.0f), m_lMatrix[indexA][2]);
	t = vector3(tx, ty, tz);
	//t *= static_cast<vector3>(vector4(t, 1.0f) * m_lMatrix[indexA]);
	//compute subexpressions. Add in epsiolon to counteract
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			AbsR[i][j] = glm::abs(R[i][j]) + FLT_EPSILON;
		}
	}

	//TEST AXES L = A0, L = A1, L = A2
	for(int i = 0; i < 3; i++) {
		radiusA = aR[i];
		radiusB = bR[0] * AbsR[i][0] + bR[1] * AbsR[i][1] + bR[2] * AbsR[i][2];
		if(abs(t[i]) > radiusA + radiusB) return false;
	}

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) {
		radiusA = aR[0] * AbsR[0][i] + aR[1] * AbsR[1][i] + aR[2] * AbsR[2][i];
		radiusB = bR[i];
		if (abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > radiusA + radiusB)
			return false;
	}

	// Test axis L = A0 x B0
	radiusA = aR[1] * AbsR[2][0] + aR[2] * AbsR[1][0];
	radiusB = bR[1] * AbsR[0][2] + bR[2] * AbsR[0][1];
	if (abs(t[2] * R[1][0] - t[1] * R[2][0]) > radiusA + radiusB) return false;

	// Test axis L = A0 x B1
	radiusA = aR[1] * AbsR[2][1] + aR[2] * AbsR[1][1];
	radiusB = bR[0] * AbsR[0][2] + bR[2] * AbsR[0][0];
	if (abs(t[2] * R[1][1] - t[1] * R[2][1]) > radiusA + radiusB) return false;

	// Test axis L = A0 x B2
	radiusA = aR[1] * AbsR[2][2] + aR[2] * AbsR[1][2];
	radiusB = bR[0] * AbsR[0][1] + bR[1] * AbsR[0][0];
	if (abs(t[2] * R[1][2] - t[1] * R[2][2]) > radiusA + radiusB) return false;

	// Test axis L = A1 x B0
	radiusA = aR[0] * AbsR[2][0] + aR[2] * AbsR[0][0];
	radiusB = bR[1] * AbsR[1][2] + bR[2] * AbsR[1][1];
	if (abs(t[0] * R[2][0] - t[2] * R[0][0]) > radiusA + radiusB) return false;

	// Test axis L = A1 x B1
	radiusA = aR[0] * AbsR[2][1] + aR[2] * AbsR[0][1];
	radiusB = bR[0] * AbsR[1][2] + bR[2] * AbsR[1][0];
	if (abs(t[0] * R[2][1] - t[2] * R[0][1]) > radiusA + radiusB) return false;

	// Test axis L = A1 x B2
	radiusA = aR[0] * AbsR[2][2] + aR[2] * AbsR[0][2];
	radiusB = bR[0] * AbsR[1][1] + bR[1] * AbsR[1][0];
	if (abs(t[0] * R[2][2] - t[2] * R[0][2]) > radiusA + radiusB) return false;
	// Test axis L = A2 x B0
	radiusA = aR[0] * AbsR[1][0] + aR[1] * AbsR[0][0];
	radiusB = bR[1] * AbsR[2][2] + bR[2] * AbsR[2][1];
	if (abs(t[1] * R[0][0] - t[0] * R[1][0]) > radiusA + radiusB) return false;
	// Test axis L = A2 x B1
	radiusA = aR[0] * AbsR[1][1] + aR[1] * AbsR[0][1];
	radiusB = bR[0] * AbsR[2][2] + bR[2] * AbsR[2][0];
	if (abs(t[1] * R[0][1] - t[0] * R[1][1]) > radiusA + radiusB) return false;
	// Test axis L = A2 x B2
	radiusA = aR[0] * AbsR[1][2] + aR[1] * AbsR[0][2];
	radiusB = bR[0] * AbsR[2][1] + bR[1] * AbsR[2][0];
	if (abs(t[1] * R[0][2] - t[0] * R[1][2]) > radiusA + radiusB) return false;

	return true;
}