#pragma once

#include "skeletal-anim-processor.h"
#include "hash.h"

namespace fbxsdk
{
class FbxAnimLayer;
}

struct HierarchyHasher : SkeletalAnimProcessor
{
	HierarchyHasher(std::vector<std::string> const& exclude, fbxsdk::FbxAnimLayer& layer);

	uint64_t GetHash() const { return mHash; }

private:
	Hash mHash;

	bool OnEnterJoint(fbxsdk::FbxNode& node) override;
};
