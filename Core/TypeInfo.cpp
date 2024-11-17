#include "TypeInfo.h"

TypeInfo::TypeInfo(NameHash typeName, std::vector<NameHash> parentTypes)
	: m_TypeName(typeName)
	, m_ParentTypes(parentTypes)
{
	TypeInfoStore::Get().RegisterTypeInfo(this);
}

bool TypeInfo::IsOrInheritsFrom(NameHash typeToCheck) const
{
	return m_TypeName == typeToCheck
		|| InheritsFrom(typeToCheck);
}

bool TypeInfo::InheritsFrom(NameHash possibleParentType) const
{
	for (NameHash myParent : m_ParentTypes)
	{
		if (possibleParentType == myParent)
		{
			return true;
		}
		else
		{
			TypeInfo const* parentInfo = TypeInfoStore::Get().FindTypeInfo(myParent);
			if (parentInfo != nullptr && parentInfo->InheritsFrom(possibleParentType))
			{
				return true;
			}
		}
	}
	return false;
}

void TypeInfoStore::RegisterTypeInfo(TypeInfo const * typeInfo)
{
	m_TypeInfoMap.emplace(typeInfo->GetName(), typeInfo);
}

TypeInfo const* TypeInfoStore::FindTypeInfo(NameHash typeName) const
{
	auto itr = m_TypeInfoMap.find(typeName);
	if (itr == m_TypeInfoMap.cend())
	{
		return nullptr;
	}
	else
	{
		return itr->second;
	}
}
