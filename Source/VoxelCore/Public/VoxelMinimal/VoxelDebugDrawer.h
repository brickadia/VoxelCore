// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/VoxelBox.h"
#include "VoxelMinimal/VoxelObjectPtr.h"
#include "VoxelMinimal/VoxelCriticalSection.h"

struct FVoxelDebugDrawGroup;

struct VOXELCORE_API FVoxelDebugPoint
{
	FVector3f Center = FVector3f(ForceInit);
	uint8 SizeInCm = 10;
	uint8 R = 0;
	uint8 G = 0;
	uint8 B = 0;
};
checkStatic(sizeof(FVoxelDebugPoint) == sizeof(FVector4f));

struct VOXELCORE_API FVoxelDebugLine
{
	FVector3f Start = FVector3f(ForceInit);
	float Padding = 0.f;
	FVector3f End = FVector3f(ForceInit);
	uint8 R = 0;
	uint8 G = 0;
	uint8 B = 0;
	uint8 A = 0;
};
checkStatic(sizeof(FVoxelDebugLine) == 2 * sizeof(FVector4f));

struct VOXELCORE_API FVoxelDebugSphere
{
	FVector3f Center = FVector3f(ForceInit);
	float Radius = 0.f;
	float Flags = 0.f;
	float Padding2 = 0.f;
	float Padding3 = 0.f;
	uint8 R = 0;
	uint8 G = 0;
	uint8 B = 0;
	uint8 A = 0;

	static constexpr float Flag_DrawCross = 1.f;
};
checkStatic(sizeof(FVoxelDebugSphere) == 2 * sizeof(FVector4f));

struct VOXELCORE_API FVoxelDebugSphericalSector
{
	FVector3f Origin = FVector3f(ForceInit);
	float Radius = 0.f;
	FVector3f Direction = FVector3f(ForceInit);
	float HalfAngle = 0.f;
	float Padding1 = 0.f;
	float Padding2 = 0.f;
	float Padding3 = 0.f;
	uint8 R = 0;
	uint8 G = 0;
	uint8 B = 0;
	uint8 A = 0;
};
checkStatic(sizeof(FVoxelDebugSphericalSector) == 3 * sizeof(FVector4f));

struct VOXELCORE_API FVoxelDebugBox
{
	FVector3f Center = FVector3f(ForceInit);
	uint8 R = 0;
	uint8 G = 0;
	uint8 B = 0;
	uint8 A = 0;
	FVector3f HalfExtent = FVector3f(ForceInit);
	float QuatW = 1.f;
	FVector3f QuatXYZ = FVector3f(ForceInit);
	float Padding = 0.f;
};
checkStatic(sizeof(FVoxelDebugBox) == 3 * sizeof(FVector4f));

struct VOXELCORE_API FVoxelDebugDraw
{
	TVoxelChunkedArray<FVoxelDebugPoint> Points;
	TVoxelChunkedArray<FVoxelDebugLine> Lines;
	TVoxelChunkedArray<FVoxelDebugSphere> Spheres;
	TVoxelChunkedArray<FVoxelDebugSphericalSector> SphericalSectors;
	TVoxelChunkedArray<FVoxelDebugBox> Boxes;
	TVoxelChunkedArray<FVoxelDebugPoint> ForegroundPoints;
	TVoxelChunkedArray<FVoxelDebugLine> ForegroundLines;
	TVoxelChunkedArray<FVoxelDebugSphere> ForegroundSpheres;
	TVoxelChunkedArray<FVoxelDebugSphericalSector> ForegroundSphericalSectors;
	TVoxelChunkedArray<FVoxelDebugBox> ForegroundBoxes;
};

// Usage: from any thread:
//
// FVoxelDebugDrawer()
// .LifeTime(0.5f) // Optional, can also use OneFrame or nothing. If nothing will be permanent until cleared with voxel.ClearDebugDraws
// .Color(FLinearColor::Red)
// .DrawPoint(Position);
//
// If you have a lot of points consider batching them in the same drawer.
// They will be drawn when the drawer is destructed/goes out of scope.
//
// FVoxelDebugDrawer Drawer;
// Drawer.Color(FLinearColor::Red);
//
// for (const FVector& Point : Points)
// {
//     Drawer.DrawPoint(Point);
// }
//
class VOXELCORE_API FVoxelDebugDrawer
{
public:
	FVoxelDebugDrawer();
	explicit FVoxelDebugDrawer(TVoxelObjectPtr<const UWorld> World);
	explicit FVoxelDebugDrawer(const UWorld* World);
	~FVoxelDebugDrawer();
	UE_NONCOPYABLE(FVoxelDebugDrawer);

public:
	FVoxelDebugDrawer& Group(const TSharedPtr<FVoxelDebugDrawGroup>& DrawGroup);

public:
	FVoxelDebugDrawer& Color(const FLinearColor& NewColor);
	FVoxelDebugDrawer& Foreground();
	FVoxelDebugDrawer& OneFrame();
	FVoxelDebugDrawer& LifeTime(float NewLifeTime);

public:
	FVoxelDebugDrawer& DrawPoint(
		const FVector& Position,
		uint8 SizeInCm = 10);

	FVoxelDebugDrawer& DrawPoint(
		FVector3f Position,
		uint8 SizeInCm = 10);

	template<typename T>
	FVoxelDebugDrawer& DrawPoint(
		const FVector& Position,
		T) = delete;

public:
	FVoxelDebugDrawer& DrawLine(
		const FVector& Start,
		const FVector& End);

	FVoxelDebugDrawer& DrawLine(
		FVector3f Start,
		FVector3f End);

	FVoxelDebugDrawer& DrawBox(
		const FVoxelBox& Box,
		const FMatrix& Transform);

	FVoxelDebugDrawer& DrawBox(
		const FVoxelBox& Box,
		const FTransform& Transform);

	FVoxelDebugDrawer& DrawBox(
		FVector3f Center,
		FVector3f HalfExtent,
		FQuat4f Rotation = FQuat4f::Identity);

	FVoxelDebugDrawer& DrawWireSphere(
		const FVector& Center,
		double Radius,
		bool bDrawCross = false);

	FVoxelDebugDrawer& DrawWireSphere(
		FVector3f Center,
		float Radius,
		bool bDrawCross = false);

	FVoxelDebugDrawer& DrawWireSphericalSector(
		const FVector& Origin,
		const FVector& Direction,
		double Radius,
		double HalfAngle);

	FVoxelDebugDrawer& DrawWireSphericalSector(
		FVector3f Origin,
		FVector3f Direction,
		float Radius,
		float HalfAngle);

	FVoxelDebugDrawer& DrawWireCone(
		const FVector& Origin,
		const FVector& Direction,
		double Length,
		double HalfAngleRad,
		int32 NumSides = 16);

	FVoxelDebugDrawer& DrawWireCone(
		FVector3f Origin,
		FVector3f Direction,
		float Length,
		float HalfAngleRad,
		int32 NumSides = 16);

private:
	const TVoxelObjectPtr<const UWorld> World;
	bool bIsOneFrame = false;
	bool bIsForeground = false;
	float PrivateLifeTime = -1;
	FColor PrivateColor = FColor::Red;
	const TSharedRef<FVoxelDebugDraw> Draw = MakeShared<FVoxelDebugDraw>();
	TSharedPtr<FVoxelDebugDrawGroup> PrivateDrawGroup;
};

struct VOXELCORE_API FVoxelDebugDrawGroup : public TSharedFromThis<FVoxelDebugDrawGroup>
{
private:
	FVoxelDebugDrawGroup() = default;

public:
	static TSharedRef<FVoxelDebugDrawGroup> Create();

public:
	void Clear_AnyThread();
	void AddDraw_AnyThread(
		bool bIsOneFrame,
		double EndTime,
		const TSharedRef<const FVoxelDebugDraw>& Draw);

public:
	void PushGroup_AnyThread();
	void PushGroup_AnyThread(TVoxelObjectPtr<const UWorld> World);
	void PushGroup_AnyThread(const UWorld* World);

	void PushGroup_EnsureNew_AnyThread();
	void PushGroup_EnsureNew_AnyThread(TVoxelObjectPtr<const UWorld> World);
	void PushGroup_EnsureNew_AnyThread(const UWorld* World);

private:
	void IterateDraws(
		double Time,
		TVoxelArray<TSharedPtr<const FVoxelDebugDraw>>& OutStaticDraws,
		TVoxelArray<TSharedPtr<const FVoxelDebugDraw>>& OutDynamicDraws);

private:
	FVoxelCriticalSection CriticalSection;

	struct FDraw
	{
		bool bIsOneFrame = false;
		double EndTime = 0;
		TSharedPtr<const FVoxelDebugDraw> Draw;
	};

	bool IsStaticDraw(const FDraw& Draw) const;

	TVoxelArray<FDraw> Draws_RequiresLock;

	std::atomic<uint64> StaticGeneration{ 0 };

	friend class FVoxelDebugDrawerWorldManager;
};