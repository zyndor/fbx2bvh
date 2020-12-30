#include "visitor.h"
#include "fbxsdk/scene/geometry/fbxnode.h"

void Visitor::Visit(fbxsdk::FbxNode& node)
{
	if (OnEnter(node))
	{
		for (int i = 0; i < node.GetChildCount(); ++i)
		{
			Visit(*node.GetChild(i));
		}
		OnExit(node);
	}
}
