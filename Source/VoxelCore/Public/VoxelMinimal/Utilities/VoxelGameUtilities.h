// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"

class FViewport;

namespace FVoxelUtilities
{
	VOXELCORE_API FViewport* GetViewport(const UWorld* World);
	VOXELCORE_API bool GetCameraView(const UWorld* World, FVector& OutPosition, FRotator& OutRotation, float& OutFOV);
	VOXELCORE_API TOptional<FVector> GetCameraPosition(const UWorld* World);

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	VOXELCORE_API bool ComputeInvokerChunks(
		TVoxelSet<FIntVector>& OutChunks,
		TVoxelArray<FSphere> Invokers,
		const FMatrix& LocalToWorld,
		double ChunkSize,
		int32 MaxNumChunks);

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
	VOXELCORE_API void OnActorSelectionChanged(
		AActor& Actor,
		TDelegate<void(bool bIsSelected)> Delegate);

	VOXELCORE_API bool IsActorSelected_AnyThread(FObjectKey Actor);
#endif

	VOXELCORE_API void CopyBodyInstance(
		FBodyInstance& Dest,
		const FBodyInstance& Source);

	VOXELCORE_API bool BodyInstanceEqual(
		const FBodyInstance& A,
		const FBodyInstance& B);
}