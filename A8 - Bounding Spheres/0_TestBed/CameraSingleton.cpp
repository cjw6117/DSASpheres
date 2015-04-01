#include "ME\Camera\CameraSingleton.h"
using namespace MyEngine;
//  CameraSingleton
CameraSingleton* CameraSingleton::m_pInstance = nullptr;
CameraSingleton* CameraSingleton::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new CameraSingleton();
	}
	return m_pInstance;
}
CameraSingleton::CameraSingleton(){Init();}
CameraSingleton::CameraSingleton(CameraSingleton const& other){}
CameraSingleton& CameraSingleton::operator=(CameraSingleton const& other) {	return *this; }
CameraSingleton::~CameraSingleton(){Release();};
void CameraSingleton::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//--- Non Standard Singleton Methods
void CameraSingleton::Release()
{
	Init();
	m_pSystem = nullptr;
}

void CameraSingleton::Init()
{
	m_pSystem = SystemSingleton::GetInstance();

	m_fFieldOfView = 45.0f;
	float y = static_cast<float> (m_pSystem->WindowWidth);
	float x = static_cast<float> (m_pSystem->WindowHeight);
	if(x == 0)
		x = 1;
	m_fAspectRatio = y / x;
	m_fNear = 0.01f;
	m_fFar = 1000.0f;
		
	// View
	m_vPosition = vector3(0.0f, 0.0f, 5.0f);
	m_vForward = vector3(0.0f, 0.0f, -1.0f);
	m_vUp = vector3(0.0f, 1.0f, 0.0f);
	
	CalculateView();
	CalculateProjection();
	
	GetMVP();
}

//Projection
void CameraSingleton::CalculateProjection()
{
	float y = static_cast<float> (m_pSystem->WindowWidth);
	float x = static_cast<float> (m_pSystem->WindowHeight);
	if(x == 0)
		x = 1;

	m_fAspectRatio = y / x;
	
	Projection = glm::perspective(	m_fFieldOfView,
									m_fAspectRatio,
									m_fNear,
									m_fFar);
}
void CameraSingleton::SetProjection(matrix4 a_mProjection)
{
	m_mProjection = a_mProjection;
}
matrix4 CameraSingleton::GetProjection()
{
	return m_mProjection;
}

//View
void CameraSingleton::CalculateView()
{
	m_mView = glm::lookAt(	m_vPosition,
							m_vPosition + m_vForward, 
							m_vUp);
}
void CameraSingleton::SetView(matrix4 a_mView)
{
	m_mView = a_mView;
}
matrix4 CameraSingleton::GetView(void)
{
	return m_mView;
}
vector3 CameraSingleton::GetForwardVector(void)
{
	return m_vForward;
}
vector3 CameraSingleton::GetUpVector(void)
{
	return m_vUp;
}
float CameraSingleton::GetFieldOfView(void)
{
	return m_fFieldOfView;
}
float CameraSingleton::GetNear(void)
{
	return m_fNear;
}
float CameraSingleton::GetFar(void)
{
	return m_fFar;
}
//MVP
matrix4 CameraSingleton::GetMVP(matrix4 a_mModel)
{
	m_mMVP = m_mProjection * m_mView * a_mModel;
	return m_mMVP;
}
matrix4 CameraSingleton::GetMVP()
{
	m_mMVP = m_mProjection * m_mView;
	return m_mMVP;
}

void CameraSingleton::Rotate(float a_fAngX, float a_fAngY)
{
	//direction: Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(a_fAngX) * sin(a_fAngY), 
		sin(a_fAngX),
		cos(a_fAngX) * cos(a_fAngY)
	);
	
	//right vector
	glm::vec3 right = glm::vec3(
		sin(a_fAngY -  PI/2.0f), 
		0,
		cos(a_fAngY - PI/2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	m_vForward = -direction;
	m_vUp = up;
	
	//SetDirection(-direction);
	//SetUp(up);
	CalculateView();
}

//Translation
void CameraSingleton::SetPosition(vector3 a_Postion)
{
	m_vPosition = a_Postion;


	vector3 v3At = a_Postion + vector3(0.0f, 0.0f, -1.0f);
	vector3 v3Up = vector3(0.0f, 1.0f, 0.0f);

	m_mView = glm::lookAt( a_Postion, v3At, v3Up );
}
vector3 CameraSingleton::GetPosition(void)
{
	return m_vPosition;
}
void CameraSingleton::SetForwardVector(vector3 a_vForward)
{
	m_vForward = a_vForward;
}
void CameraSingleton::SetUpVector(vector3 a_vDirection)
{
	m_vUp = a_vDirection;
}
void CameraSingleton::MoveForward(float a_fDisntance)
{
	m_vPosition += m_vForward * a_fDisntance;
}
void CameraSingleton::MoveVertical(float a_fDisntance)
{
	m_vPosition += m_vUp * a_fDisntance;
}

void CameraSingleton::MoveSideways(float a_fDisntance)
{
	vector3 right = glm::cross(m_vForward, m_vUp);
	m_vPosition += right * a_fDisntance;
}

void CameraSingleton::SetView(vector3 a_Target)
{
	m_mView = glm::lookAt(	m_vPosition,
							a_Target, 
							m_vUp);

	m_vForward = a_Target - m_vPosition;
	m_vForward = glm::normalize(m_vForward);
}

void CameraSingleton::PrintInfo(void)
{
	printf("Cam: ");
	printf("P( %.3f, %.3f, %.3f) ", m_vPosition.x, m_vPosition.y, m_vPosition.z);
	printf("V( %.3f, %.3f, %.3f) ", m_vForward.x, m_vForward.y, m_vForward.z);
}