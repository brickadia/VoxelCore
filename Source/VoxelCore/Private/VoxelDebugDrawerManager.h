// Copyright Voxel Plugin SAS. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

class FVoxelDebugDrawerWorldManager : public IVoxelWorldSubsystem
{
public:
	GENERATED_VOXEL_WORLD_SUBSYSTEM_BODY(FVoxelDebugDrawerWorldManager);

public:
	// Clears all global and grouped draws
	void ClearAll_AnyThread();

	FORCEINLINE const TSharedRef<FVoxelDebugDrawGroup>& GetGlobalGroup_AnyThread() const
	{
		return GlobalGroup;
	}

	void AddGroup_AnyThread(const TSharedRef<FVoxelDebugDrawGroup>& Group);
	void AddGroup_EnsureNew_AnyThread(const TSharedRef<FVoxelDebugDrawGroup>& Group);

public:
	void RenderPoints_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots* OverrideRenderTargets = nullptr,
		FRHIDepthStencilState* OverrideDepthStencilState = nullptr);

	void RenderLines_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots* OverrideRenderTargets = nullptr,
		FRHIDepthStencilState* OverrideDepthStencilState = nullptr);

	void RenderBoxes_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots* OverrideRenderTargets = nullptr,
		FRHIDepthStencilState* OverrideDepthStencilState = nullptr);

	void RenderSphericalSectors_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots* OverrideRenderTargets = nullptr,
		FRHIDepthStencilState* OverrideDepthStencilState = nullptr);

	void RenderSpheres_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots* OverrideRenderTargets = nullptr,
		FRHIDepthStencilState* OverrideDepthStencilState = nullptr);

	void RenderForegroundPoints_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots& RenderTargets,
		FRHIDepthStencilState* DepthStencilState);

	void RenderForegroundLines_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots& RenderTargets,
		FRHIDepthStencilState* DepthStencilState);

	void RenderForegroundBoxes_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots& RenderTargets,
		FRHIDepthStencilState* DepthStencilState);

	void RenderForegroundSphericalSectors_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots& RenderTargets,
		FRHIDepthStencilState* DepthStencilState);

	void RenderForegroundSpheres_RenderThread(
		FRDGBuilder& GraphBuilder,
		FViewInfo& View,
		const FRenderTargetBindingSlots& RenderTargets,
		FRHIDepthStencilState* DepthStencilState);

public:
	//~ Begin IVoxelWorldSubsystem Interface
	virtual void Tick() override;
	//~ End IVoxelWorldSubsystem Interface

public:
	struct FBufferSet
	{
		TRefCountPtr<FRDGPooledBuffer> PooledPointBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugPoint>> UploadedPointsToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugPoint>> PointsToRender_RenderThread;
		FRDGBufferSRVRef PointBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledLineBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugLine>> UploadedLinesToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugLine>> LinesToRender_RenderThread;
		FRDGBufferSRVRef LineBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledForegroundPointBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugPoint>> UploadedForegroundPointsToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugPoint>> ForegroundPointsToRender_RenderThread;
		FRDGBufferSRVRef ForegroundPointBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledForegroundLineBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugLine>> UploadedForegroundLinesToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugLine>> ForegroundLinesToRender_RenderThread;
		FRDGBufferSRVRef ForegroundLineBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledBoxBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugBox>> UploadedBoxesToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugBox>> BoxesToRender_RenderThread;
		FRDGBufferSRVRef BoxBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledForegroundBoxBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugBox>> UploadedForegroundBoxesToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugBox>> ForegroundBoxesToRender_RenderThread;
		FRDGBufferSRVRef ForegroundBoxBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledSphericalSectorBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugSphericalSector>> UploadedSphericalSectorsToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugSphericalSector>> SphericalSectorsToRender_RenderThread;
		FRDGBufferSRVRef SphericalSectorBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledForegroundSphericalSectorBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugSphericalSector>> UploadedForegroundSphericalSectorsToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugSphericalSector>> ForegroundSphericalSectorsToRender_RenderThread;
		FRDGBufferSRVRef ForegroundSphericalSectorBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledSphereBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugSphere>> UploadedSpheresToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugSphere>> SpheresToRender_RenderThread;
		FRDGBufferSRVRef SphereBufferSRV = nullptr;

		TRefCountPtr<FRDGPooledBuffer> PooledForegroundSphereBuffer;
		TWeakPtr<const TVoxelArray<FVoxelDebugSphere>> UploadedForegroundSpheresToRender;
		TSharedPtr<const TVoxelArray<FVoxelDebugSphere>> ForegroundSpheresToRender_RenderThread;
		FRDGBufferSRVRef ForegroundSphereBufferSRV = nullptr;
	};

	struct FPendingBufferData
	{
		bool bStaticChanged = false;

		TVoxelArray<FVoxelDebugPoint> DynPoints;
		TVoxelArray<FVoxelDebugLine> DynLines;
		TVoxelArray<FVoxelDebugSphere> DynSpheres;
		TVoxelArray<FVoxelDebugSphericalSector> DynSectors;
		TVoxelArray<FVoxelDebugBox> DynBoxes;
		TVoxelArray<FVoxelDebugPoint> DynFgPoints;
		TVoxelArray<FVoxelDebugLine> DynFgLines;
		TVoxelArray<FVoxelDebugSphere> DynFgSpheres;
		TVoxelArray<FVoxelDebugSphericalSector> DynFgSectors;
		TVoxelArray<FVoxelDebugBox> DynFgBoxes;

		TSharedPtr<TVoxelArray<FVoxelDebugPoint>> StatPoints;
		TSharedPtr<TVoxelArray<FVoxelDebugLine>> StatLines;
		TSharedPtr<TVoxelArray<FVoxelDebugSphere>> StatSpheres;
		TSharedPtr<TVoxelArray<FVoxelDebugSphericalSector>> StatSectors;
		TSharedPtr<TVoxelArray<FVoxelDebugBox>> StatBoxes;
		TSharedPtr<TVoxelArray<FVoxelDebugPoint>> StatFgPoints;
		TSharedPtr<TVoxelArray<FVoxelDebugLine>> StatFgLines;
		TSharedPtr<TVoxelArray<FVoxelDebugSphere>> StatFgSpheres;
		TSharedPtr<TVoxelArray<FVoxelDebugSphericalSector>> StatFgSectors;
		TSharedPtr<TVoxelArray<FVoxelDebugBox>> StatFgBoxes;
	};

	void KickFlatten(uint32 FrameNumber);
	void WaitForFlattenAndApply(uint32 FrameNumber);

	uint32 LastKickFrameNumber = ~0u;

	struct FPendingFrame
	{
		FVoxelFuture Flatten;
		std::atomic<bool> bReady{ false };
		bool bApplied = false;
		TSharedPtr<FPendingBufferData> BufferData;
	};

	static constexpr int32 NumPendingFrames = 3;
	FPendingFrame PendingFrames[NumPendingFrames];

	FBufferSet StaticBuffers;
	FBufferSet DynamicBuffers;

	const UWorld* World_Unsafe = nullptr;

	FVoxelCriticalSection CriticalSection;

	TSharedRef<FVoxelDebugDrawGroup> GlobalGroup = MakeShareable<FVoxelDebugDrawGroup>(new FVoxelDebugDrawGroup());
	TVoxelSet<TWeakPtr<FVoxelDebugDrawGroup>> Groups_RequiresLock = { GlobalGroup };

	uint64 LastStaticGeneration = 0;

	friend class FVoxelDebugDrawerManager;
};

class FVoxelDebugDrawerManager : public FVoxelRenderSingleton
{
public:
	TVoxelObjectPtr<UWorld> DefaultWorld;

	//~ Begin FVoxelRenderSingleton Interface
	virtual void Tick() override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& ViewFamily) override;

	virtual void PostRenderBasePassDeferred_RenderThread(
		FRDGBuilder& GraphBuilder,
		FSceneView& View,
		const FRenderTargetBindingSlots& RenderTargets,
		TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures) override;

	virtual bool ShouldCompositeEditorPrimitives_RenderThread(const FSceneView& View) const override;
	virtual void PrepareEditorPrimitiveResources_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, FSceneViewExtensionBase::FEditorPrimitiveBufferBindings& Bindings) override;
	virtual void RenderEditorPrimitivesOpaque_RenderThread(FRHICommandList& RHICmdList, const FSceneView& View) override;
	virtual void RenderEditorPrimitivesForegroundOverwrite_RenderThread(FRHICommandList& RHICmdList, const FSceneView& View) override;
	virtual void RenderEditorPrimitivesForegroundDepthTest_RenderThread(FRHICommandList& RHICmdList, const FSceneView& View) override;
	//~ End FVoxelRenderSingleton Interface

private:
	static void PrepareBuffersForWorld(FRDGBuilder& GraphBuilder, const FSceneView& View, FSceneViewExtensionBase::FEditorPrimitiveBufferBindings& Bindings);
	static void RenderEditorPrimitivesForWorld(FRHICommandList& RHICmdList, const FSceneView& View, bool bForeground, FRHIDepthStencilState* DepthStencilState);
};
extern FVoxelDebugDrawerManager* GVoxelDebugDrawerManager;