// Copyright Voxel Plugin SAS. All Rights Reserved.

#include "VoxelMinimal.h"
#include "Net/RepLayout.h"
#include "Engine/NetConnection.h"
#include "UObject/CoreRedirects.h"
#include "Engine/PackageMapClient.h"
#include "UObject/UObjectThreadContext.h"
#include "StructUtils/StructUtilsTypes.h"
#include "StructUtils/StructReinstancer.h"
#include "StructUtils/UserDefinedStruct.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void FVoxelInstancedStruct::InitializeAs(UScriptStruct* NewScriptStruct, const void* NewStructMemory)
{
	*this = {};

	if (!NewScriptStruct)
	{
		// Null
		ensure(!NewStructMemory);
		return;
	}

	PrivateScriptStruct = NewScriptStruct;
	PrivateStructMemory = MakeSharedStruct(NewScriptStruct, NewStructMemory);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelInstancedStruct::Reset()
{
	*this = FVoxelInstancedStruct();
}

struct FVoxelInstancedStructStructOnScope : public FStructOnScope
{
	FSharedVoidPtr Data;

	using FStructOnScope::FStructOnScope;
};

TSharedRef<FStructOnScope> FVoxelInstancedStruct::MakeStructOnScope()
{
	const TSharedRef<FVoxelInstancedStructStructOnScope> Result = MakeShared<FVoxelInstancedStructStructOnScope>(
		GetScriptStruct(),
		static_cast<uint8*>(GetStructMemory()));

	Result->Data = PrivateStructMemory;

	return Result;
}

bool FVoxelInstancedStruct::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	VOXEL_FUNCTION_COUNTER();

	uint8 bValidData = Ar.IsSaving() ? IsValid() : 0;
	Ar.SerializeBits(&bValidData, 1);

	if (!bValidData)
	{
		if (Ar.IsLoading())
		{
			Reset();
		}
		bOutSuccess = true;
		return true;
	}

	if (Ar.IsLoading())
	{
		UScriptStruct* NewScriptStruct = nullptr;
		Ar << NewScriptStruct;

		if (PrivateScriptStruct != NewScriptStruct)
		{
			*this = FVoxelInstancedStruct(NewScriptStruct);
		}

		if (!IsValid())
		{
			UE_LOG(LogCore, Error, TEXT("FInstancedStruct::NetSerialize: Bad script struct serialized, cannot recover."));
			Ar.SetError();
			bOutSuccess = false;
		}
	}
	else
	{
		Ar << PrivateScriptStruct;
	}

	if (!PrivateScriptStruct)
	{
		return true;
	}

	if (EnumHasAllFlags(PrivateScriptStruct->StructFlags, STRUCT_NetSerializeNative))
	{
		PrivateScriptStruct->GetCppStructOps()->NetSerialize(Ar, Map, bOutSuccess, GetStructMemory());
	}
	else
	{
		bOutSuccess = INLINE_LAMBDA
		{
			UPackageMapClient* MapClient = Cast<UPackageMapClient>(Map);
			check(::IsValid(MapClient));

			UNetConnection* NetConnection = MapClient->GetConnection();
			check(::IsValid(NetConnection));
			check(::IsValid(NetConnection->GetDriver()));

			const TSharedPtr<FRepLayout> RepLayout = NetConnection->GetDriver()->GetStructRepLayout(PrivateScriptStruct);
			check(RepLayout.IsValid());

			bool bHasUnmapped = false;
			RepLayout->SerializePropertiesForStruct(PrivateScriptStruct, static_cast<FBitArchive&>(Ar), Map, GetStructMemory(), bHasUnmapped);
			return true;
		};
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelInstancedStruct::Serialize(FArchive& Ar)
{
	VOXEL_FUNCTION_COUNTER();

	SerializeVoxelVersion(Ar);

	using FVersion = DECLARE_VOXEL_VERSION
	(
		InitialVersion,
		StoreName
	);

	uint8 Version = FVersion::LatestVersion;
	Ar << Version;
	ensure(Version <= FVersion::LatestVersion);

	if (Ar.IsLoading())
	{
		FString StructPath;
		if (Version >= FVersion::StoreName)
		{
			Ar << StructPath;
		}

		{
			UScriptStruct* NewScriptStruct = nullptr;
			Ar << NewScriptStruct;

			if (!NewScriptStruct &&
				StructPath != TEXTVIEW("<null>"))
			{
				VOXEL_SCOPE_COUNTER("FindFirstObject");

				// Serializing structs directly doesn't seem to handle redirects properly
				const FCoreRedirectObjectName RedirectedName = FCoreRedirects::GetRedirectedName(
					ECoreRedirectFlags::Type_Struct,
					FCoreRedirectObjectName(StructPath),
					ECoreRedirectMatchFlags::AllowPartialMatch);

				NewScriptStruct = FindFirstObject<UScriptStruct>(*RedirectedName.ToString(), EFindFirstObjectOptions::EnsureIfAmbiguous);
			}

			if (NewScriptStruct)
			{
				Ar.Preload(NewScriptStruct);
			}

			if (NewScriptStruct != GetScriptStruct())
			{
				InitializeAs(NewScriptStruct);
			}
			ensure(GetScriptStruct() == NewScriptStruct);
		}

		int32 SerializedSize = 0;
		Ar << SerializedSize;

		if (!GetScriptStruct() && SerializedSize > 0)
		{
			LOG_VOXEL(Warning, "Struct %s not found. Archive: %s Callstack: \n%s",
				*StructPath,
				*FVoxelUtilities::GetArchivePath(Ar),
				*FVoxelUtilities::GetPrettyCallstack_WithStats());

			Ar.Seek(Ar.Tell() + SerializedSize);
		}
		else if (GetScriptStruct())
		{
			const int64 Start = Ar.Tell();
			GetScriptStruct()->SerializeItem(Ar, GetStructMemory(), nullptr);
			ensure(Ar.Tell() - Start == SerializedSize);

			if (GetScriptStruct() == StaticStructFast<FBodyInstance>())
			{
				static_cast<FBodyInstance*>(GetStructMemory())->LoadProfileData(false);
			}
		}
	}
	else if (
		Ar.IsSaving() ||
		// Make sure object references are accounted for
		// This is critical for FindReferences or example packaging to work
		Ar.IsObjectReferenceCollector() ||
		Ar.IsCountingMemory())
	{
		UScriptStruct* ScriptStructToSerialize = GetScriptStruct();

#if WITH_EDITOR
		if (const UUserDefinedStruct* UserDefinedStruct = Cast<UUserDefinedStruct>(ScriptStructToSerialize))
		{
			// See FInstancedStruct::Serialize
			// See FVoxelInstancedStruct::AddStructReferencedObjects

			if (UserDefinedStruct->Status == UDSS_Duplicate &&
				UserDefinedStruct->PrimaryStruct.IsValid())
			{
				// If saving a duplicated UDS, save the primary type instead, so that the data is loaded with the original struct.
				// This is used as part of the user defined struct reinstancing logic.
				ScriptStructToSerialize = UserDefinedStruct->PrimaryStruct.Get();
			}
		}
#endif

		FString StructPath;
		if (ScriptStructToSerialize)
		{
			StructPath = ScriptStructToSerialize->GetPathName();
		}
		else
		{
			StructPath = "<null>";
		}

		Ar << StructPath;
		Ar << ScriptStructToSerialize;

		const int64 SerializedSizePosition = Ar.Tell();
		{
			int32 SerializedSize = 0;
			Ar << SerializedSize;
		}

		const int64 Start = Ar.Tell();
		if (GetScriptStruct())
		{
			check(GetStructMemory());

			if (GetScriptStruct()->IsChildOf(StaticStructFast<FVoxelVirtualStruct>()))
			{
				static_cast<FVoxelVirtualStruct*>(GetStructMemory())->PreSerialize();
			}

			GetScriptStruct()->SerializeItem(Ar, GetStructMemory(), nullptr);
		}
		const int64 End = Ar.Tell();

		Ar.Seek(SerializedSizePosition);
		{
			int32 SerializedSize = End - Start;
			Ar << SerializedSize;
		}
		Ar.Seek(End);
	}

	return true;
}

bool FVoxelInstancedStruct::Identical(const FVoxelInstancedStruct* Other, const uint32 PortFlags) const
{
	if (!ensure(Other))
	{
		return false;
	}

	if (!IsValid() &&
		!Other->IsValid())
	{
		return true;
	}

	if (GetScriptStruct() != Other->GetScriptStruct())
	{
		return false;
	}

	VOXEL_SCOPE_COUNTER_FORMAT("CompareScriptStruct %s", *GetScriptStruct()->GetName());
	return GetScriptStruct()->CompareScriptStruct(GetStructMemory(), Other->GetStructMemory(), PortFlags);
}

bool FVoxelInstancedStruct::ExportTextItem(FString& ValueStr, const FVoxelInstancedStruct& DefaultValue, UObject* Parent, const int32 PortFlags, UObject* ExportRootScope) const
{
	VOXEL_FUNCTION_COUNTER();

	if (!IsValid())
	{
		ValueStr += "None";
		return true;
	}

	ValueStr += GetScriptStruct()->GetPathName();

	// ALWAYS provide a defaults, otherwise FProperty::Identical assumes the default is 0
	const FSharedVoidRef Defaults = MakeSharedStruct(GetScriptStruct());

	GetScriptStruct()->ExportText(
		ValueStr,
		GetStructMemory(),
		GetScriptStruct() == DefaultValue.GetScriptStruct()
		? DefaultValue.GetStructMemory()
		: &Defaults.Get(),
		Parent,
		PortFlags,
		ExportRootScope);

	return true;
}

bool FVoxelInstancedStruct::ImportTextItem(const TCHAR*& Buffer, const int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText, FArchive* InSerializingArchive)
{
	VOXEL_FUNCTION_COUNTER();

	FString StructPathName;
	{
		const TCHAR* Result = FPropertyHelpers::ReadToken(Buffer, StructPathName, true);
		if (!Result)
		{
			return false;
		}
		Buffer = Result;
	}

	if (StructPathName.Len() == 0 ||
		StructPathName == "None")
	{
		*this = {};
		return true;
	}

	UScriptStruct* NewScriptStruct = LoadObject<UScriptStruct>(nullptr, *StructPathName);
	if (!ensure(NewScriptStruct))
	{
		return false;
	}

	if (NewScriptStruct != GetScriptStruct())
	{
		InitializeAs(NewScriptStruct);
	}

	{
		const TCHAR* Result = NewScriptStruct->ImportText(
			Buffer,
			GetStructMemory(),
			Parent,
			PortFlags,
			ErrorText,
			[&] { return NewScriptStruct->GetName(); });

		if (!Result)
		{
			return false;
		}
		Buffer = Result;
	}

	return true;
}

void FVoxelInstancedStruct::AddStructReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();

	if (!GetScriptStruct())
	{
		return;
	}

#if WITH_EDITOR
	// See FInstancedStruct::AddStructReferencedObjects

	// Reference collector is used to visit all instances of instanced structs and replace their contents.
	if (UE::StructUtils::FStructReinstancer* Reinstancer = UE::StructUtils::FStructReinstancer::GetInstance())
	{
		if (const UScriptStruct* ScriptStruct = GetScriptStruct();
			ScriptStruct && Reinstancer->IsReinstanting(ScriptStruct))
		{
			Reinstancer->MarkAsRequiresReinstantiation();
			// It is not necessary to change the struct to a duplicated one, but it helps with tracking mistakes and prevents memory stomps.
			if (const UScriptStruct* Duplicated = Reinstancer->GetDuplicatedReinstantingStruct(ScriptStruct))
			{
				PrivateScriptStruct = ConstCast(Duplicated);
			}
		}
	}
#endif

	TObjectPtr<UScriptStruct> ScriptStructObject = GetScriptStruct();
	Collector.AddReferencedObject(ScriptStructObject);
	check(ScriptStructObject);

	FVoxelUtilities::AddStructReferencedObjects(Collector, FVoxelStructView(*this));
}

void FVoxelInstancedStruct::GetPreloadDependencies(TArray<UObject*>& OutDependencies) const
{
	if (GetScriptStruct())
	{
		OutDependencies.Add(GetScriptStruct());
	}
}