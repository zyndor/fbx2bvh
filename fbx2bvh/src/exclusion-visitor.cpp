#include "exclusion-visitor.h"
#include "fbxsdk/scene/geometry/fbxnode.h"

ExclusionVisitor::ExclusionVisitor(std::vector<std::string> const& excludes)
:	mExcludes(excludes)
{}

bool ExclusionVisitor::OnEnter(fbxsdk::FbxNode & node)
{
	return !IsExcluded(node.GetName()) && OnEnterIncluded(node);
}

bool ExclusionVisitor::IsExcluded(std::string const& name) const
{
	for (auto& e : mExcludes)
	{
		if (!e.empty())
		{
			bool anyStart = *e.begin() == '*';
			bool anyEnd = *e.rbegin() == '*';
			if (anyStart && anyEnd)
			{
				if (name.find(e.c_str() + 1, 0, e.size() - 2) != std::string::npos)
				{
					return true;
				}
			}
			else if (anyStart)
			{
				if (name.rfind(e.c_str(), 0, e.size() - 1) == name.length() - e.length())
				{
					return true;
				}
			}
			else if (anyEnd)
			{
				if (name.find(e.c_str(), 0, e.size() - 1) == 0)
				{
					return true;
				}
			}
			else if (name == e)
			{
				return true;
			}
		}
	}
	return false;
}
