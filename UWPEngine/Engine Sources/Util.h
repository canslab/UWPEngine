#pragma once

namespace JHUtil
{
	extern const UINT VENDOR_ID_NVIDIA;
	extern const UINT VENDOR_ID_INTEL;
	extern const UINT VENDOR_ID_AMD;
	extern const UINT VENDOR_ID_QUALCOMM;

	Platform::Array<byte>^ LoadShaderFile(std::string shaderFile);
}