#pragma once

namespace fbxsdk
{
class FbxManager;
class FbxScene;
}

struct FbxSdk
{
	fbxsdk::FbxManager* mManager;
	fbxsdk::FbxScene* mScene;

	FbxSdk();
	~FbxSdk();

	bool LoadScene(char const* filename);
};