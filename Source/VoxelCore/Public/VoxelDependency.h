// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

class FVoxelAABBTree;
class FVoxelDependency;
class FVoxelDependencyTracker;

DECLARE_VOXEL_MEMORY_STAT(VOXELCORE_API, STAT_VoxelDependencies, "Dependencies");

struct FVoxelDependencyInvalidationParameters
{
	TSharedPtr<const FVoxelAABBTree> Bounds;
};

class VOXELCORE_API FVoxelDependencyInvalidationScope
{
public:
	FVoxelDependencyInvalidationScope();
	~FVoxelDependencyInvalidationScope();

private:
	struct FInvalidation
	{
		TSharedPtr<FVoxelDependency> Dependency;
		FVoxelDependencyInvalidationParameters Parameters;
	};
	TVoxelChunkedArray<FInvalidation> Invalidations;

	void Invalidate();

	friend FVoxelDependency;
};

class VOXELCORE_API FVoxelDependency : public TSharedFromThis<FVoxelDependency>
{
public:
	const FString Name;

	static TSharedRef<FVoxelDependency> Create(const FString& Name)
	{
		return TSharedRef<FVoxelDependency>(new FVoxelDependency(Name));
	}
	~FVoxelDependency() = default;
	UE_NONCOPYABLE(FVoxelDependency);

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelDependencies);

	int64 GetAllocatedSize() const
	{
		return TrackerRefs_RequiresLock.GetAllocatedSize();
	}

public:
	TTSMulticastDelegate<void(FVoxelDependencyInvalidationParameters Parameters)> OnInvalidated;

	void Invalidate(const FVoxelDependencyInvalidationParameters& Parameters = {});
	void Invalidate(const FVoxelBox& Bounds);
	void Invalidate(TConstVoxelArrayView<FVoxelBox> Bounds);

private:
	FVoxelCriticalSection CriticalSection;

	struct FTrackerRef
	{
		TWeakPtr<FVoxelDependencyTracker> WeakTracker;

		bool bHasBounds = false;
		FVoxelBox Bounds;

		FORCEINLINE bool operator==(const FTrackerRef& Other) const
		{
			if (WeakTracker != Other.WeakTracker ||
				bHasBounds != Other.bHasBounds)
			{
				return false;
			}

			if (bHasBounds &&
				Bounds != Other.Bounds)
			{
				return false;
			}

			return true;
		}
	};
	TVoxelChunkedSparseArray<FTrackerRef> TrackerRefs_RequiresLock;

	explicit FVoxelDependency(const FString& Name);

	void GetInvalidatedTrackers(
		const FVoxelDependencyInvalidationParameters& Parameters,
		TVoxelSet<TWeakPtr<FVoxelDependencyTracker>>& OutTrackers);

	friend FVoxelDependencyTracker;
	friend FVoxelDependencyInvalidationScope;
};