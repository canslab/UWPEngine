#pragma once
//*******************************************************
//
//		 Camera Class 
//
//	author	: Jangho Park 
//	date	: 11/11/2016
//	email	: devjh1106@gmail.com
//
//*******************************************************

class CCamera
{
public:
	DirectX::XMFLOAT4 GetPositionW() const;

public:
	bool Initialize(const std::vector<float>& positionW, const std::vector<float>& targetPositionW, const std::vector<float>& upVectorW);
	bool IsInitialized() const;

	void SetPositionW(float x, float y, float z);
	void SetTargetPositionW(float x, float y, float z);
	void SetUpVectorW(float x, float y, float z);
	
	void MovePositionBy(float dx, float dy, float dz);
	void MovePositionByOffsetX(float dx);
	void MovePositionByOffsetY(float dy);
	void MovePositionByOffsetZ(float dz);
	
	DirectX::XMFLOAT4X4 GetViewMatrix() const;

public:
	CCamera();
	virtual ~CCamera();

	// prohibit copy constructor, copy assignment operator
	CCamera(const CCamera& rhs) = delete;
	CCamera& operator=(const CCamera& rhs) = delete;

private:
	bool			  m_bInitialized;
	DirectX::XMFLOAT4 m_positionW;		// Camera Position relative to World Space
	DirectX::XMFLOAT4 m_targetW;		// Target Position
	DirectX::XMFLOAT4 m_upW;			// Up Vector, usually it is the (0, 1, 0, 0)
};

