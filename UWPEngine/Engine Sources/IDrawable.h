#pragma once
// Interface 
#include <d3d11.h>
#include <DirectXMath.h>

class IDrawable
{
public:
	virtual bool						GetNextVertex(float *resultX, float *resultY, float *resultZ) = 0;
	
	virtual unsigned int				GetVertexByteSize() const = 0;
	virtual unsigned int				GetVertexCount() const = 0;
	virtual void*						GetAddressOfVertexArray() const = 0;

	virtual unsigned int				GetIndexCount() const = 0;
	virtual void*						GetAddressOfIndexArray() const = 0;
	
	virtual DirectX::XMMATRIX			GetWorldMatrix() const = 0;
};

