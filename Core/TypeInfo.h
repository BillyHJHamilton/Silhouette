#pragma once

#include <unordered_map>
#include <vector>
#include "Core.h"
#include "NameHash.h"

class TypeInfo
{
public:
	TypeInfo(NameHash typeName, std::vector<NameHash> parentTypes = {});
	NameHash GetName() const { return m_TypeName; }
	bool IsOrInheritsFrom(NameHash typeToCheck) const;
	bool InheritsFrom(NameHash possibleParentType) const;

private:
	NameHash m_TypeName;
	std::vector<NameHash> m_ParentTypes;
};

class TypeInfoStore
{
public:
	static TypeInfoStore& Get()
	{
		static TypeInfoStore s_Instance;
		return s_Instance;
	}

	TypeInfo const* FindTypeInfo(NameHash typeName) const;

private:
	friend class TypeInfo;
	void RegisterTypeInfo(TypeInfo const* typeInfo);

	std::unordered_map<NameHash, TypeInfo const*> m_TypeInfoMap;
};

class ITypeInfoProvider
{
public:
	virtual ~ITypeInfoProvider() {}
	virtual TypeInfo const& GetTypeInfo() const = 0;
	NameHash GetTypeName() const { return GetTypeInfo().GetName(); }
};

#define MACRO_DeclareTypeInfo(ThisTypeName) \
	static TypeInfo const s_TypeInfo; \
	virtual TypeInfo const& GetTypeInfo() const override { return s_TypeInfo; } \
	static NameHash StaticType() { return s_TypeInfo.GetName(); }

#define MACRO_DefineTypeInfo(ThisTypeName) \
	TypeInfo const ThisTypeName::s_TypeInfo = TypeInfo( NameHash(#ThisTypeName), {} );

#define MACRO_DefineTypeInfoOneParent(ThisTypeName,ParentName) \
	TypeInfo const ThisTypeName::s_TypeInfo = TypeInfo( NameHash(#ThisTypeName), {NameHash(#ParentName)} );

template<typename NewType, typename OldType>
NewType* Downcast(OldType* objectToCast)
{
	if (objectToCast &&
		objectToCast->GetTypeInfo().IsOrInheritsFrom(NewType::StaticType()))
	{
		return static_cast<NewType*>(objectToCast);
	}
	else
	{
		return nullptr;
	}
}
