#pragma once

namespace fbxsdk
{
class FbxNode;
}

struct Visitor
{
	virtual ~Visitor() {}

	void Visit(fbxsdk::FbxNode& node);

protected:
	// children are only processed and OnExit() is only called if this returns true.
	virtual bool OnEnter(fbxsdk::FbxNode& node) = 0;

	virtual void OnExit(fbxsdk::FbxNode& node) = 0;
};