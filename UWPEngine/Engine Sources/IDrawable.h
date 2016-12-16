#pragma once
// Interface 
#include <d3d11.h>
#include <DirectXMath.h>

class IDrawable
{
public:
	struct DrawInfo
	{
		UINT indexCount;
		UINT startIndex;
		UINT baseVertexLocation;
	};

	virtual const void*									GetContVertexArray() const = 0;
	virtual const void*									GetContIndexArray() const = 0;
	virtual unsigned int								GetIndexCount() const = 0;
	virtual const std::vector<unsigned int>&			GetIndicesOfVertexBuffer() const = 0;
	virtual const std::vector<unsigned int>&			GetIndicesOfIndexBuffer() const = 0;
	
	virtual unsigned int 								GetTotalVertexBufferSizeInByte() const = 0;
	virtual unsigned int								GetTotalIndexBufferSizeInByte() const = 0;
	virtual unsigned int								GetVertexStride() const = 0;

	virtual const std::vector<DirectX::XMFLOAT4X4>&		GetWorldMatrices() const = 0;
	virtual unsigned int								GetNumberOfDrawableObject() const = 0;
	virtual DirectX::XMFLOAT4X4							GetCameraMatrix() const = 0;

	virtual DrawInfo									operator[](UINT idx) const = 0;
};

