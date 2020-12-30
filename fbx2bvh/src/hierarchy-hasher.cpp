#include "hierarchy-hasher.h"
#include "fbxsdk/scene/geometry/fbxnode.h"
#include "fbxsdk/scene/geometry/fbxnodeattribute.h"
#include "fbxsdk/scene/animation/fbxanimlayer.h"

HierarchyHasher::HierarchyHasher(std::vector<std::string> const& exclude, fbxsdk::FbxAnimLayer& layer)
:	SkeletalAnimProcessor(exclude, layer)
{}

bool HierarchyHasher::OnEnterJoint(fbxsdk::FbxNode& node)
{
	mHash.Add(node.GetName());

	uint32_t channelCount = 0;
	for (auto c : {
#define DECL_CHANNEL(source, axis, ignore) (node.source).GetCurve(&mAnimLayer, #axis),
#include "channels.inc"
#undef DECL_CHANNEL
		})
	{
		channelCount += !!c;
	}

	mHash.Add(channelCount);

	if (0 == node.GetChildCount())
	{
		mHash.Add(false);
	}

	return true;
}
