#include "BoundingBoxClass.h"
//  BoundingSphereClass
void BoundingBoxClass::Init(void)
{
	m_bInitialized = false;
	m_bIsVisible = true;
	min = vector3(0.0f);
	max= vector3(0.0f);
	m_v3Centroid = vector3(0.0f);
	m_sName = "NULL";
}
void BoundingBoxClass::Swap(BoundingBoxClass& other)
{
	std::swap(m_bInitialized, other.m_bInitialized);
	
	std::swap(m_v3Centroid, other.m_v3Centroid);
	std::swap(m_sName, other.m_sName);
}
void BoundingBoxClass::Release(void)
{
	//No pointers to release
}
//The big 3
BoundingBoxClass::BoundingBoxClass(){Init();}
BoundingBoxClass::BoundingBoxClass(BoundingBoxClass const& other)
{
	m_bInitialized = other.m_bInitialized;
	
	m_v3Centroid = other.m_v3Centroid;
	m_sName = other.m_sName;
}
BoundingBoxClass& BoundingBoxClass::operator=(BoundingBoxClass const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		BoundingBoxClass temp(other);
		Swap(temp);
	}
	return *this;
}
BoundingBoxClass::~BoundingBoxClass(){Release();};
//Accessors
bool BoundingBoxClass::IsInitialized(void){ return m_bInitialized; }

vector3 BoundingBoxClass::GetCentroid(void){ return m_v3Centroid; }
String BoundingBoxClass::GetName(void){return m_sName;}
//Methods
void BoundingBoxClass::GenerateBoundingBox(String a_sInstanceName)
{
	//If this has already been initialized there is nothing to do here
	if(m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{
		m_sName = a_sInstanceName;
		
		std::vector<vector3> lVertices = pMeshMngr->GetVertices(m_sName);
		unsigned int nVertices = lVertices.size();
		m_v3Centroid = lVertices[0];
		max = lVertices[0];
		min =  lVertices[0];
		for(unsigned int nVertex = 1; nVertex < nVertices; nVertex++)
		{
			//m_v3Centroid += lVertices[nVertex];
			if(min.x > lVertices[nVertex].x)
				min.x = lVertices[nVertex].x;
			else if(max.x < lVertices[nVertex].x)
				max.x = lVertices[nVertex].x;
			
			if(min.y > lVertices[nVertex].y)
				min.y = lVertices[nVertex].y;
			else if(max.y < lVertices[nVertex].y)
				max.y = lVertices[nVertex].y;

			if(min.z > lVertices[nVertex].z)
				min.z = lVertices[nVertex].z;
			else if(max.z < lVertices[nVertex].z)
				max.z = lVertices[nVertex].z;
		}
		m_v3Centroid = (min + max) / 2.0f;

		
		
		m_bInitialized = true;
	}
}
void BoundingBoxClass::AddBoxToRenderList(matrix4 a_mModelToWorld, vector3 a_vColor, bool a_bRenderCentroid)
{
	if(!m_bInitialized)
		return;
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	if(a_bRenderCentroid)
		pMeshMngr->AddAxisToQueue(a_mModelToWorld * glm::translate(m_v3Centroid));
	float scaleX = max.x - min.x;
	float scaleY = max.y - min.y;
	float scaleZ = max.z - min.z;
	pMeshMngr->AddCubeToQueue(a_mModelToWorld * glm::translate(m_v3Centroid) * glm::scale(vector3( scaleX,scaleY,scaleZ)), a_vColor, MERENDER::WIRE);
}

vector3 BoundingBoxClass::GetMin()
{
	return min;
}

vector3 BoundingBoxClass::GetMax()
{
	return max;
}

vector3 BoundingBoxClass::GetColor(void)
{
	return color;
}

void BoundingBoxClass::SetColor(vector3 color)
{
	this->color = color;
}

bool BoundingBoxClass::GetVisible(void)
{
	return m_bIsVisible;
}

void BoundingBoxClass::SetVisible(bool visible)
{
	m_bIsVisible = visible;
}