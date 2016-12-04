#include "pch.h"
#include "Util.h"
#include <fstream>
using namespace Platform;
using namespace std;

namespace JHUtil
{
	const UINT VENDOR_ID_NVIDIA = 4318;
	const UINT VENDOR_ID_INTEL = 32902;
	const UINT VENDOR_ID_AMD = 4098;
	const UINT VENDOR_ID_QUALCOMM = 1297040209;

	Platform::Array<byte>^ LoadShaderFile(std::string shaderFile)
	{
		Platform::Array<byte>^ fileData = nullptr;
		
		ifstream vertexFile(shaderFile, std::ios::in | std::ios::binary | std::ios::ate);

		if (vertexFile.is_open())
		{
			int length = (int)vertexFile.tellg();

			fileData = ref new Array<byte>(length);
			vertexFile.seekg(0, std::ios::beg);
			vertexFile.read(reinterpret_cast<char*>(fileData->Data), length);
			vertexFile.close();
		}

		return fileData;
	}
}