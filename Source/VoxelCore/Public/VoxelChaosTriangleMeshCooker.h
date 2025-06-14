﻿// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#if !BR_OBLITERATE_CHAOS

#include "VoxelMinimal.h"

struct VOXELCORE_API FVoxelChaosTriangleMeshCooker
{
	static TRefCountPtr<Chaos::FTriangleMeshImplicitObject> Create(
		TConstVoxelArrayView<int32> Indices,
		TConstVoxelArrayView<FVector3f> Vertices,
		TConstVoxelArrayView<uint16> FaceMaterials);

	static int64 GetAllocatedSize(const Chaos::FTriangleMeshImplicitObject& TriangleMesh);
};

#endif // !BR_OBLITERATE_CHAOS
