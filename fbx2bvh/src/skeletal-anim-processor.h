#pragma once

#include "exclusion-visitor.h"

namespace fbxsdk
{
class FbxAnimLayer;
}

struct SkeletalAnimProcessor : ExclusionVisitor
{
	SkeletalAnimProcessor(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer);

protected:
	fbxsdk::FbxAnimLayer& mAnimLayer;

	virtual bool OnEnterJoint(fbxsdk::FbxNode& node) = 0;

	bool OnEnterIncluded(fbxsdk::FbxNode & node) final override;
	void OnExit(fbxsdk::FbxNode & node) override;
};