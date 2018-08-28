// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "SpatialMemoryReader.h"

#include "SpatialPackageMapClient.h"
#include "WeakObjectPtr.h"
#include "SchemaHelpers.h"

void FSpatialMemoryReader::DeserializeObjectRef(UnrealObjectRef& ObjectRef)
{
	*this << ObjectRef.Entity;
	*this << ObjectRef.Offset;

	uint8 HasPath;
	SerializeBits(&HasPath, 1);
	if (HasPath)
	{
		FString Path;
		*this << Path;

		ObjectRef.Path = std::string(TCHAR_TO_UTF8(*Path));
	}

	uint8 HasOuter;
	SerializeBits(&HasOuter, 1);
	if (HasOuter)
	{
		UnrealObjectRef Outer;
		DeserializeObjectRef(Outer);

		ObjectRef.Outer = Outer;
	}
}



FArchive& FSpatialMemoryReader::operator<<(UObject*& Value)
{
	UnrealObjectRef ObjectRef;

	DeserializeObjectRef(ObjectRef);

	check(ObjectRef != SpatialConstants::UNRESOLVED_OBJECT_REF);
	if (ObjectRef == SpatialConstants::NULL_OBJECT_REF)
	{
		Value = nullptr;
	}
	else
	{
		FNetworkGUID NetGUID = PackageMap->GetNetGUIDFromUnrealObjectRef(ObjectRef);
		if (NetGUID.IsValid())
		{
			Value = PackageMap->GetObjectFromNetGUID(NetGUID, true);
			checkf(Value, TEXT("An object ref %s should map to a valid object."), *ObjectRef.ToString());
		}
		else
		{
			// TODO: Handle unresolved object refs on the receiving end
			Value = nullptr;
		}
	}

	return *this;
}

FArchive& FSpatialMemoryReader::operator<<(FWeakObjectPtr& Value)
{
	UObject* Object;
	*this << Object;

	Value = Object;

	return *this;
}

void FSpatialNetBitReader::DeserializeObjectRef(UnrealObjectRef& ObjectRef)
{
	*this << ObjectRef.Entity;
	*this << ObjectRef.Offset;

	uint8 HasPath;
	SerializeBits(&HasPath, 1);
	if (HasPath)
	{
		FString Path;
		*this << Path;

		ObjectRef.Path = std::string(TCHAR_TO_UTF8(*Path));
	}

	uint8 HasOuter;
	SerializeBits(&HasOuter, 1);
	if (HasOuter)
	{
		ObjectRef.Outer = UnrealObjectRef();
		DeserializeObjectRef(*ObjectRef.Outer);
	}
}

FArchive& FSpatialNetBitReader::operator<<(UObject*& Value)
{
	UnrealObjectRef ObjectRef;

	DeserializeObjectRef(ObjectRef);

	check(ObjectRef != UNRESOLVED_OBJECT_REF);
	if (ObjectRef == NULL_OBJECT_REF)
	{
		Value = nullptr;
	}
	else
	{
		auto PackageMapClient = Cast<USpatialPackageMapClient>(PackageMap);
		FNetworkGUID NetGUID = PackageMapClient->GetNetGUIDFromUnrealObjectRef(ObjectRef);
		if (NetGUID.IsValid())
		{
			Value = PackageMapClient->GetObjectFromNetGUID(NetGUID, true);
			checkf(Value, TEXT("An object ref %s should map to a valid object."), *ObjectRef.ToString());
		}
		else
		{
			if (PackageMapClient->bShouldTrackUnresolvedRefs)
			{
				PackageMapClient->TrackedUnresolvedRefs.Add(ObjectRef);
			}
			Value = nullptr;
		}
	}

	return *this;
}

FArchive& FSpatialNetBitReader::operator<<(FWeakObjectPtr& Value)
{
	UObject* Object;
	*this << Object;

	Value = Object;

	return *this;
}
