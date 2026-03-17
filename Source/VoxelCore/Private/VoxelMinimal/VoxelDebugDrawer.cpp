// Copyright Voxel Plugin SAS. All Rights Reserved.

#include "VoxelMinimal.h"
#include "VoxelTaskContext.h"
#include "VoxelDebugDrawerManager.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELCORE_API, bool, GVoxelFreezeDebugDraws, false,
	"voxel.FreezeDebugDraws",
	"Freeze timed debug draws so they never expire");

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDebugDrawer::FVoxelDebugDrawer()
	: World(GVoxelDebugDrawerManager->DefaultWorld)
{
}

FVoxelDebugDrawer::FVoxelDebugDrawer(const TVoxelObjectPtr<const UWorld> World)
	: World(World)
{
}

FVoxelDebugDrawer::FVoxelDebugDrawer(const UWorld* World)
	: FVoxelDebugDrawer(MakeVoxelObjectPtr(World))
{
}

FVoxelDebugDrawer::~FVoxelDebugDrawer()
{
	VOXEL_FUNCTION_COUNTER();

	if (PrivateDrawGroup)
	{
		PrivateDrawGroup->AddDraw_AnyThread(
			bIsOneFrame,
			PrivateLifeTime == -1 ? MAX_dbl : (FPlatformTime::Seconds() + PrivateLifeTime),
			Draw);
		return;
	}

	if (const TSharedPtr<FVoxelDebugDrawGroup>& DrawGroup = FVoxelTaskScope::GetContext().DrawGroup)
	{
		DrawGroup->AddDraw_AnyThread(
			bIsOneFrame,
			PrivateLifeTime == -1 ? MAX_dbl : (FPlatformTime::Seconds() + PrivateLifeTime),
			Draw);
		return;
	}

	FVoxelDebugDrawerWorldManager::Get(World)->GetGlobalGroup_AnyThread()->AddDraw_AnyThread(
		bIsOneFrame,
		PrivateLifeTime == -1 ? MAX_dbl : (FPlatformTime::Seconds() + PrivateLifeTime),
		Draw);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDebugDrawer& FVoxelDebugDrawer::Group(const TSharedPtr<FVoxelDebugDrawGroup>& DrawGroup)
{
	PrivateDrawGroup = DrawGroup;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDebugDrawer& FVoxelDebugDrawer::Color(const FLinearColor& NewColor)
{
	PrivateColor = NewColor.ToFColor(false);
	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::Foreground()
{
	bIsForeground = true;
	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::OneFrame()
{
	bIsOneFrame = true;
	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::LifeTime(const float NewLifeTime)
{
	PrivateLifeTime = NewLifeTime;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawPoint(
	const FVector& Position,
	const uint8 SizeInCm)
{
	const FVoxelDebugPoint Point
	{
		FVector3f(Position),
		SizeInCm,
		PrivateColor.R,
		PrivateColor.G,
		PrivateColor.B
	};

	if (bIsForeground)
	{
		Draw->ForegroundPoints.Add(Point);
	}
	else
	{
		Draw->Points.Add(Point);
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawPoint(
	const FVector3f Position,
	const uint8 SizeInCm)
{
	const FVoxelDebugPoint Point{ Position, SizeInCm, PrivateColor.R, PrivateColor.G, PrivateColor.B };

	if (bIsForeground)
	{
		Draw->ForegroundPoints.Add(Point);
	}
	else
	{
		Draw->Points.Add(Point);
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawLine(
	const FVector& Start,
	const FVector& End)
{
	const FVoxelDebugLine Line
	{
		FVector3f(Start),
		0.f,
		FVector3f(End),
		PrivateColor.R,
		PrivateColor.G,
		PrivateColor.B
	};

	if (bIsForeground)
	{
		Draw->ForegroundLines.Add(Line);
	}
	else
	{
		Draw->Lines.Add(Line);
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawLine(
	const FVector3f Start,
	const FVector3f End)
{
	const FVoxelDebugLine Line{ Start, 0.f, End, PrivateColor.R, PrivateColor.G, PrivateColor.B };

	if (bIsForeground)
	{
		Draw->ForegroundLines.Add(Line);
	}
	else
	{
		Draw->Lines.Add(Line);
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawBox(
	const FVoxelBox& Box,
	const FMatrix& Transform)
{
	return DrawBox(Box, FTransform{ Transform });
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawBox(
	const FVoxelBox& Box,
	const FTransform& Transform)
{
	VOXEL_FUNCTION_COUNTER();

	if (Box.IsInfinite())
	{
		return *this;
	}

	const FVector LocalCenter = (Box.Min + Box.Max) * 0.5;
	const FVector LocalHalfExtent = (Box.Max - Box.Min) * 0.5;
	const FVector WorldCenter = Transform.TransformPosition(LocalCenter);
	const FVector3f ScaledHalfExtent = FVector3f(LocalHalfExtent * Transform.GetScale3D());
	const FQuat4f Rotation{ Transform.GetRotation() };

	const FVoxelDebugBox BoxData
	{
		FVector3f{ WorldCenter },
		PrivateColor.R, PrivateColor.G, PrivateColor.B, 0,
		ScaledHalfExtent,
		Rotation.W,
		FVector3f{ Rotation.X, Rotation.Y, Rotation.Z },
		0.f
	};

	if (bIsForeground)
	{
		Draw->ForegroundBoxes.Add(BoxData);
	}
	else
	{
		Draw->Boxes.Add(BoxData);
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawBox(
	const FVector3f Center,
	const FVector3f HalfExtent,
	const FQuat4f Rotation)
{
	const FVoxelDebugBox BoxData
	{
		Center,
		PrivateColor.R, PrivateColor.G, PrivateColor.B, 0,
		HalfExtent,
		Rotation.W,
		FVector3f{ Rotation.X, Rotation.Y, Rotation.Z },
		0.f
	};

	if (bIsForeground)
	{
		Draw->ForegroundBoxes.Add(BoxData);
	}
	else
	{
		Draw->Boxes.Add(BoxData);
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawWireSphere(
	const FVector& Center,
	const double Radius,
	const bool bDrawCross)
{
	return DrawWireSphere(FVector3f(Center), static_cast<float>(Radius), bDrawCross);
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawWireSphere(
	const FVector3f Center,
	const float Radius,
	const bool bDrawCross)
{
	const FVoxelDebugSphere Sphere{ Center, Radius, bDrawCross ? FVoxelDebugSphere::Flag_DrawCross : 0.f, 0.f, 0.f, PrivateColor.R, PrivateColor.G, PrivateColor.B };

	if (bIsForeground)
	{
		Draw->ForegroundSpheres.Add(Sphere);
	}
	else
	{
		Draw->Spheres.Add(Sphere);
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawWireSphericalSector(
	const FVector& Origin,
	const FVector& Direction,
	const double Radius,
	const double HalfAngle)
{
	return DrawWireSphericalSector(FVector3f(Origin), FVector3f(Direction), static_cast<float>(Radius), static_cast<float>(HalfAngle));
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawWireSphericalSector(
	const FVector3f Origin,
	const FVector3f Direction,
	const float Radius,
	const float HalfAngle)
{
	const FVoxelDebugSphericalSector Sector{ Origin, Radius, Direction, HalfAngle, 0.f, 0.f, 0.f, PrivateColor.R, PrivateColor.G, PrivateColor.B };

	if (bIsForeground)
	{
		Draw->ForegroundSphericalSectors.Add(Sector);
	}
	else
	{
		Draw->SphericalSectors.Add(Sector);
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawWireCone(
	const FVector& Origin,
	const FVector& Direction,
	const double Length,
	const double HalfAngleRad,
	const int32 NumSides)
{
	const FVector Dir = Direction.GetSafeNormal();
	FVector Up, Right;
	Dir.FindBestAxisVectors(Right, Up);

	const double ConeRadius = Length * FMath::Tan(HalfAngleRad);
	const FVector Tip = Origin;
	const FVector BaseCenter = Origin + Dir * Length;
	const double AngleStep = 2.0 * UE_DOUBLE_PI / NumSides;

	FVector PrevPoint = BaseCenter + Right * ConeRadius;

	for (int32 i = 1; i <= NumSides; ++i)
	{
		const double Angle = i * AngleStep;
		const FVector Point = BaseCenter + (Right * FMath::Cos(Angle) + Up * FMath::Sin(Angle)) * ConeRadius;

		// Base circle segment
		DrawLine(PrevPoint, Point);

		// Side line from tip
		if (i % FMath::Max(1, NumSides / 4) == 0)
		{
			DrawLine(Tip, Point);
		}

		PrevPoint = Point;
	}

	return *this;
}

FVoxelDebugDrawer& FVoxelDebugDrawer::DrawWireCone(
	const FVector3f Origin,
	const FVector3f Direction,
	const float Length,
	const float HalfAngleRad,
	const int32 NumSides)
{
	const FVector3f Dir = Direction.GetSafeNormal();
	FVector3f Up, Right;
	Dir.FindBestAxisVectors(Right, Up);

	const float ConeRadius = Length * FMath::Tan(HalfAngleRad);
	const FVector3f BaseCenter = Origin + Dir * Length;
	const float AngleStep = 2.f * UE_PI / NumSides;

	FVector3f PreviousPoint = BaseCenter + Right * ConeRadius;

	for (int32 SideIndex = 1; SideIndex <= NumSides; ++SideIndex)
	{
		const float Angle = SideIndex * AngleStep;
		const FVector3f Point = BaseCenter + (Right * FMath::Cos(Angle) + Up * FMath::Sin(Angle)) * ConeRadius;

		DrawLine(PreviousPoint, Point);

		if (SideIndex % FMath::Max(1, NumSides / 4) == 0)
		{
			DrawLine(Origin, Point);
		}

		PreviousPoint = Point;
	}

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FVoxelDebugDrawGroup> FVoxelDebugDrawGroup::Create()
{
	return MakeShareable(new FVoxelDebugDrawGroup());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDebugDrawGroup::Clear_AnyThread()
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	Draws_RequiresLock.Empty();
	StaticGeneration.fetch_add(1, std::memory_order_relaxed);
}

void FVoxelDebugDrawGroup::AddDraw_AnyThread(
	const bool bIsOneFrame,
	const double EndTime,
	const TSharedRef<const FVoxelDebugDraw>& Draw)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	const FDraw NewDraw{ bIsOneFrame, EndTime, Draw };
	Draws_RequiresLock.Add(NewDraw);

	if (IsStaticDraw(NewDraw))
	{
		StaticGeneration.fetch_add(1, std::memory_order_relaxed);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDebugDrawGroup::PushGroup_AnyThread()
{
	FVoxelDebugDrawerWorldManager::Get(GVoxelDebugDrawerManager->DefaultWorld)->AddGroup_AnyThread(AsShared());
}

void FVoxelDebugDrawGroup::PushGroup_AnyThread(const TVoxelObjectPtr<const UWorld> World)
{
	FVoxelDebugDrawerWorldManager::Get(World)->AddGroup_AnyThread(AsShared());
}

void FVoxelDebugDrawGroup::PushGroup_AnyThread(const UWorld* World)
{
	FVoxelDebugDrawerWorldManager::Get(World)->AddGroup_AnyThread(AsShared());
}

void FVoxelDebugDrawGroup::PushGroup_EnsureNew_AnyThread()
{
	FVoxelDebugDrawerWorldManager::Get(GVoxelDebugDrawerManager->DefaultWorld)->AddGroup_EnsureNew_AnyThread(AsShared());
}

void FVoxelDebugDrawGroup::PushGroup_EnsureNew_AnyThread(const TVoxelObjectPtr<const UWorld> World)
{
	FVoxelDebugDrawerWorldManager::Get(World)->AddGroup_EnsureNew_AnyThread(AsShared());
}

void FVoxelDebugDrawGroup::PushGroup_EnsureNew_AnyThread(const UWorld* World)
{
	FVoxelDebugDrawerWorldManager::Get(World)->AddGroup_EnsureNew_AnyThread(AsShared());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelDebugDrawGroup::IsStaticDraw(const FDraw& Draw) const
{
	return !Draw.bIsOneFrame && Draw.EndTime == MAX_dbl;
}

void FVoxelDebugDrawGroup::IterateDraws(
	const double Time,
	TVoxelArray<TSharedPtr<const FVoxelDebugDraw>>& OutStaticDraws,
	TVoxelArray<TSharedPtr<const FVoxelDebugDraw>>& OutDynamicDraws)
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	for (int32 Index = 0; Index < Draws_RequiresLock.Num(); Index++)
	{
		const FDraw& Draw = Draws_RequiresLock[Index];

		if (IsStaticDraw(Draw))
		{
			OutStaticDraws.Add(Draw.Draw);
		}
		else
		{
			// Always render at least once
			OutDynamicDraws.Add(Draw.Draw);

			if (Draw.bIsOneFrame ||
				(!GVoxelFreezeDebugDraws && Draw.EndTime < Time))
			{
				Draws_RequiresLock.RemoveAtSwap(Index);
				Index--;
			}
		}
	}
}