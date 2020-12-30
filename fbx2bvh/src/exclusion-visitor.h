#pragma once

#include "visitor.h"
#include <vector>
#include <string>

struct ExclusionVisitor : Visitor
{
	ExclusionVisitor(std::vector<std::string> const& excludes);

	bool OnEnter(fbxsdk::FbxNode& node) final override;

protected:
	virtual bool OnEnterIncluded(fbxsdk::FbxNode& node) = 0;

private:
	std::vector<std::string> const& mExcludes;

	bool IsExcluded(std::string const& name) const;
};
