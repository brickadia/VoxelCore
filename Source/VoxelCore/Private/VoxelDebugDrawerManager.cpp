// Copyright Voxel Plugin SAS. All Rights Reserved.

#include "VoxelDebugDrawerManager.h"
#include "SceneRendering.h"
#include "SceneView.h"
#include "RHIResourceUtils.h"

VOXEL_CONSOLE_COMMAND(
	"voxel.ClearDebugDraws",
	"Clear all debug draws")
{
	for (const TSharedRef<FVoxelDebugDrawerWorldManager>& Manager : FVoxelDebugDrawerWorldManager::GetAll())
	{
		Manager->ClearAll_AnyThread();
	}
}

FVoxelDebugDrawerManager* GVoxelDebugDrawerManager = new FVoxelDebugDrawerManager();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_SHADER_PARAMETER_STRUCT(FVoxelDebugPointParameters,)
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
	SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, PointDataBuffer)
END_SHADER_PARAMETER_STRUCT()

BEGIN_SHADER_PARAMETER_STRUCT(FVoxelDebugLineParameters,)
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
	SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, LineDataBuffer)
	SHADER_PARAMETER_ARRAY(FVector4f, FrustumPlanes, [4])
END_SHADER_PARAMETER_STRUCT()

class FVoxelDebugPointVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FVoxelDebugPointVS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugPointVS, FGlobalShader);
	using FParameters = FVoxelDebugPointParameters;
};

class FVoxelDebugPointPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FVoxelDebugPointPS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugPointPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_INCLUDE(FVoxelDebugPointParameters, Common)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

class FVoxelDebugLineVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FVoxelDebugLineVS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugLineVS, FGlobalShader);
	using FParameters = FVoxelDebugLineParameters;
};

class FVoxelDebugLinePS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FVoxelDebugLinePS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugLinePS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_INCLUDE(FVoxelDebugLineParameters, Common)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

BEGIN_SHADER_PARAMETER_STRUCT(FVoxelDebugSphereParameters,)
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
	SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, SphereDataBuffer)
	SHADER_PARAMETER_ARRAY(FVector4f, FrustumPlanes, [4])
END_SHADER_PARAMETER_STRUCT()

class FVoxelDebugSphereVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FVoxelDebugSphereVS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugSphereVS, FGlobalShader);
	using FParameters = FVoxelDebugSphereParameters;
};

class FVoxelDebugSpherePS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FVoxelDebugSpherePS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugSpherePS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_INCLUDE(FVoxelDebugSphereParameters, Common)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FVoxelDebugPointVS, "/Plugin/Voxel/VoxelDebugDraw.usf", "PointVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FVoxelDebugPointPS, "/Plugin/Voxel/VoxelDebugDraw.usf", "PointPS", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FVoxelDebugLineVS, "/Plugin/Voxel/VoxelDebugDraw.usf", "LineVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FVoxelDebugLinePS, "/Plugin/Voxel/VoxelDebugDraw.usf", "LinePS", SF_Pixel);
BEGIN_SHADER_PARAMETER_STRUCT(FVoxelDebugBoxParameters,)
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
	SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, BoxDataBuffer)
	SHADER_PARAMETER_ARRAY(FVector4f, FrustumPlanes, [4])
END_SHADER_PARAMETER_STRUCT()

class FVoxelDebugBoxVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FVoxelDebugBoxVS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugBoxVS, FGlobalShader);
	using FParameters = FVoxelDebugBoxParameters;
};

class FVoxelDebugBoxPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FVoxelDebugBoxPS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugBoxPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_INCLUDE(FVoxelDebugBoxParameters, Common)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

BEGIN_SHADER_PARAMETER_STRUCT(FVoxelDebugSphericalSectorParameters,)
	SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
	SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, SectorDataBuffer)
	SHADER_PARAMETER_ARRAY(FVector4f, FrustumPlanes, [4])
END_SHADER_PARAMETER_STRUCT()

class FVoxelDebugSphericalSectorVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FVoxelDebugSphericalSectorVS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugSphericalSectorVS, FGlobalShader);
	using FParameters = FVoxelDebugSphericalSectorParameters;
};

class FVoxelDebugSphericalSectorPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FVoxelDebugSphericalSectorPS);
	SHADER_USE_PARAMETER_STRUCT(FVoxelDebugSphericalSectorPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_STRUCT_INCLUDE(FVoxelDebugSphericalSectorParameters, Common)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FVoxelDebugSphereVS, "/Plugin/Voxel/VoxelDebugDraw.usf", "SphereVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FVoxelDebugSpherePS, "/Plugin/Voxel/VoxelDebugDraw.usf", "SpherePS", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FVoxelDebugBoxVS, "/Plugin/Voxel/VoxelDebugDraw.usf", "BoxVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FVoxelDebugBoxPS, "/Plugin/Voxel/VoxelDebugDraw.usf", "BoxPS", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FVoxelDebugSphericalSectorVS, "/Plugin/Voxel/VoxelDebugDraw.usf", "SphericalSectorVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FVoxelDebugSphericalSectorPS, "/Plugin/Voxel/VoxelDebugDraw.usf", "SphericalSectorPS", SF_Pixel);

DECLARE_GPU_STAT(VoxelDebugDrawPoints);
DECLARE_GPU_STAT(VoxelDebugDrawLines);
DECLARE_GPU_STAT(VoxelDebugDrawSpheres);
DECLARE_GPU_STAT(VoxelDebugDrawBoxes);
DECLARE_GPU_STAT(VoxelDebugDrawSphericalSectors);

static constexpr int32 NUM_SPHERE_SEGMENTS = 24;
static constexpr int32 NUM_SPHERE_CIRCLES = 3;
static constexpr int32 NUM_SPHERE_CROSS_LINES = 3;
static constexpr int32 QUADS_PER_SPHERE = NUM_SPHERE_CIRCLES * NUM_SPHERE_SEGMENTS + NUM_SPHERE_CROSS_LINES;
static constexpr int32 VERTS_PER_SPHERE = QUADS_PER_SPHERE * 4;
static constexpr int32 INDICES_PER_SPHERE = QUADS_PER_SPHERE * 6;

class FVoxelDebugSphereIndexBuffer : public FIndexBuffer
{
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override
	{
		uint16 Indices[INDICES_PER_SPHERE];

		for (int32 Quad = 0; Quad < QUADS_PER_SPHERE; ++Quad)
		{
			const uint16 Base = static_cast<uint16>(Quad * 4);
			Indices[Quad * 6 + 0] = Base + 0;
			Indices[Quad * 6 + 1] = Base + 1;
			Indices[Quad * 6 + 2] = Base + 2;
			Indices[Quad * 6 + 3] = Base + 2;
			Indices[Quad * 6 + 4] = Base + 1;
			Indices[Quad * 6 + 5] = Base + 3;
		}

		IndexBufferRHI = UE::RHIResourceUtils::CreateIndexBufferFromArray(RHICmdList, TEXT("VoxelDebugSphereIB"), EBufferUsageFlags::Static, MakeConstArrayView(Indices));
	}
};

static TGlobalResource<FVoxelDebugSphereIndexBuffer> GVoxelDebugSphereIndexBuffer;

static constexpr int32 QUADS_PER_BOX = 12;
static constexpr int32 VERTS_PER_BOX = QUADS_PER_BOX * 4;
static constexpr int32 INDICES_PER_BOX = QUADS_PER_BOX * 6;

class FVoxelDebugBoxIndexBuffer : public FIndexBuffer
{
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override
	{
		uint16 Indices[INDICES_PER_BOX];

		for (int32 Quad = 0; Quad < QUADS_PER_BOX; ++Quad)
		{
			const uint16 Base = static_cast<uint16>(Quad * 4);
			Indices[Quad * 6 + 0] = Base + 0;
			Indices[Quad * 6 + 1] = Base + 1;
			Indices[Quad * 6 + 2] = Base + 2;
			Indices[Quad * 6 + 3] = Base + 2;
			Indices[Quad * 6 + 4] = Base + 1;
			Indices[Quad * 6 + 5] = Base + 3;
		}

		IndexBufferRHI = UE::RHIResourceUtils::CreateIndexBufferFromArray(RHICmdList, TEXT("VoxelDebugBoxIB"), EBufferUsageFlags::Static, MakeConstArrayView(Indices));
	}
};

static TGlobalResource<FVoxelDebugBoxIndexBuffer> GVoxelDebugBoxIndexBuffer;

static constexpr int32 NUM_SECTOR_BASE_SEGMENTS = 24;
static constexpr int32 NUM_SECTOR_SIDE_LINES = 4;
static constexpr int32 NUM_SECTOR_ARC_SEGMENTS = 16;
static constexpr int32 NUM_SECTOR_ARCS = 2;
static constexpr int32 QUADS_PER_SECTOR = NUM_SECTOR_BASE_SEGMENTS + NUM_SECTOR_SIDE_LINES + NUM_SECTOR_ARCS * NUM_SECTOR_ARC_SEGMENTS;
static constexpr int32 VERTS_PER_SECTOR = QUADS_PER_SECTOR * 4;
static constexpr int32 INDICES_PER_SECTOR = QUADS_PER_SECTOR * 6;

class FVoxelDebugSphericalSectorIndexBuffer : public FIndexBuffer
{
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override
	{
		uint16 Indices[INDICES_PER_SECTOR];

		for (int32 Quad = 0; Quad < QUADS_PER_SECTOR; ++Quad)
		{
			const uint16 Base = static_cast<uint16>(Quad * 4);
			Indices[Quad * 6 + 0] = Base + 0;
			Indices[Quad * 6 + 1] = Base + 1;
			Indices[Quad * 6 + 2] = Base + 2;
			Indices[Quad * 6 + 3] = Base + 2;
			Indices[Quad * 6 + 4] = Base + 1;
			Indices[Quad * 6 + 5] = Base + 3;
		}

		IndexBufferRHI = UE::RHIResourceUtils::CreateIndexBufferFromArray(RHICmdList, TEXT("VoxelDebugSphericalSectorIB"), EBufferUsageFlags::Static, MakeConstArrayView(Indices));
	}
};

static TGlobalResource<FVoxelDebugSphericalSectorIndexBuffer> GVoxelDebugSphericalSectorIndexBuffer;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDebugDrawerWorldManager::ClearAll_AnyThread()
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	for (auto It = Groups_RequiresLock.CreateIterator(); It; ++It)
	{
		const TSharedPtr<FVoxelDebugDrawGroup> Group = It->Pin();
		if (!Group)
		{
			It.RemoveCurrent();
			continue;
		}

		Group->Clear_AnyThread();
	}
}

void FVoxelDebugDrawerWorldManager::AddGroup_AnyThread(const TSharedRef<FVoxelDebugDrawGroup>& Group)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	Groups_RequiresLock.Add(Group);
}

void FVoxelDebugDrawerWorldManager::AddGroup_EnsureNew_AnyThread(const TSharedRef<FVoxelDebugDrawGroup>& Group)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	Groups_RequiresLock.Add_EnsureNew(Group);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void RenderPointsInternal(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const TSharedPtr<const TVoxelArray<FVoxelDebugPoint>>& PointsData,
	TRefCountPtr<FRDGPooledBuffer>& PooledBuffer,
	TWeakPtr<const TVoxelArray<FVoxelDebugPoint>>& UploadedData,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState,
	const TCHAR* EventName,
	bool bWriteAlpha = false)
{
	ensure(IsInRenderingThread());

	const int32 NumPoints = PointsData ? PointsData->Num() : 0;
	if (NumPoints == 0)
	{
		PooledBuffer = {};
		return;
	}

	VOXEL_FUNCTION_COUNTER_NUM(NumPoints);

	if (!PooledBuffer ||
		int64(PooledBuffer->Desc.NumElements) < NumPoints)
	{
		VOXEL_SCOPE_COUNTER("Create buffer");

		const FRDGBufferRef Buffer = GraphBuilder.CreateBuffer(
			FRDGBufferDesc::CreateBufferDesc(sizeof(FVoxelDebugPoint), NumPoints),
			TEXT("VoxelDebugDraw.Points"));

		PooledBuffer = GraphBuilder.ConvertToExternalBuffer(Buffer);
	}

	const FRDGBufferRef PointBuffer = GraphBuilder.RegisterExternalBuffer(PooledBuffer);

	if (UploadedData != PointsData)
	{
		UploadedData = PointsData;

		FVoxelUtilities::UploadBuffer(
			GraphBuilder,
			PointBuffer,
			PointsData->View<uint8>(),
			MakeSharedVoidPtr(PointsData));
	}

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.FeatureLevel);
	FVoxelDebugPointPS::FParameters* Parameters = GraphBuilder.AllocParameters<FVoxelDebugPointPS::FParameters>();

	if (OverrideRenderTargets)
	{
		Parameters->RenderTargets = *OverrideRenderTargets;
	}
	else
	{
		const FSceneTextures& SceneTextures = View.GetSceneTextures();

		Parameters->RenderTargets.DepthStencil = FDepthStencilBinding(
			SceneTextures.Depth.Target,
			ERenderTargetLoadAction::ELoad,
			ERenderTargetLoadAction::ELoad,
			FExclusiveDepthStencil::DepthWrite_StencilWrite);

		Parameters->RenderTargets[0] = FRenderTargetBinding(SceneTextures.Color.Target, ERenderTargetLoadAction::ELoad);

		if (SceneTextures.GBufferB)
		{
			Parameters->RenderTargets[1] = FRenderTargetBinding(SceneTextures.GBufferB, ERenderTargetLoadAction::ELoad);
		}
	}

	Parameters->Common.View = View.ViewUniformBuffer;
	Parameters->Common.PointDataBuffer = GraphBuilder.CreateSRV(PointBuffer, PF_A32B32G32R32F);

	const TShaderMapRef<FVoxelDebugPointVS> VertexShader(ShaderMap);
	const TShaderMapRef<FVoxelDebugPointPS> PixelShader(ShaderMap);

	RDG_EVENT_SCOPE_STAT(GraphBuilder, VoxelDebugDrawPoints, "VoxelDebugDrawPoints");

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("%s", EventName),
		Parameters,
		ERDGPassFlags::Raster,
		[VertexShader, PixelShader, &View, Parameters, NumPoints, OverrideDepthStencilState, bWriteAlpha](FRDGAsyncTask, FRHICommandList& RHICmdList)
		{
			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			RHICmdList.SetViewport(
				View.ViewRect.Min.X, View.ViewRect.Min.Y, 0.0f,
				View.ViewRect.Max.X, View.ViewRect.Max.Y, 1.0f);

			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
			GraphicsPSOInit.DepthStencilState = OverrideDepthStencilState
				? OverrideDepthStencilState
				: TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI();
			GraphicsPSOInit.BlendState = TStaticBlendState<
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				true>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;

			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GEmptyVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), Parameters->Common);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *Parameters);

			RHICmdList.DrawIndexedPrimitive(
				GTwoTrianglesIndexBuffer.IndexBufferRHI,
				0,
				0,
				4,
				0,
				2,
				NumPoints);
		});
}

static void RenderLinesInternal(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const TSharedPtr<const TVoxelArray<FVoxelDebugLine>>& LinesData,
	TRefCountPtr<FRDGPooledBuffer>& PooledBuffer,
	TWeakPtr<const TVoxelArray<FVoxelDebugLine>>& UploadedData,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState,
	const TCHAR* EventName,
	bool bWriteAlpha = false)
{
	ensure(IsInRenderingThread());

	const int32 NumLines = LinesData ? LinesData->Num() : 0;
	if (NumLines == 0)
	{
		PooledBuffer = {};
		return;
	}

	VOXEL_FUNCTION_COUNTER_NUM(NumLines);

	if (!PooledBuffer ||
		int64(PooledBuffer->Desc.NumElements) < int64(NumLines) * 2)
	{
		VOXEL_SCOPE_COUNTER("Create buffer");

		const FRDGBufferRef Buffer = GraphBuilder.CreateBuffer(
			FRDGBufferDesc::CreateBufferDesc(sizeof(FVector4f), NumLines * 2),
			TEXT("VoxelDebugDraw.Lines"));

		PooledBuffer = GraphBuilder.ConvertToExternalBuffer(Buffer);
	}

	const FRDGBufferRef LineBuffer = GraphBuilder.RegisterExternalBuffer(PooledBuffer);

	if (UploadedData != LinesData)
	{
		UploadedData = LinesData;

		FVoxelUtilities::UploadBuffer(
			GraphBuilder,
			LineBuffer,
			LinesData->View<uint8>(),
			MakeSharedVoidPtr(LinesData));
	}

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.FeatureLevel);

	FVoxelDebugLinePS::FParameters* Parameters = GraphBuilder.AllocParameters<FVoxelDebugLinePS::FParameters>();

	if (OverrideRenderTargets)
	{
		Parameters->RenderTargets = *OverrideRenderTargets;
	}
	else
	{
		const FSceneTextures& SceneTextures = View.GetSceneTextures();

		Parameters->RenderTargets.DepthStencil = FDepthStencilBinding(
			SceneTextures.Depth.Target,
			ERenderTargetLoadAction::ELoad,
			ERenderTargetLoadAction::ELoad,
			FExclusiveDepthStencil::DepthWrite_StencilWrite);

		Parameters->RenderTargets[0] = FRenderTargetBinding(SceneTextures.Color.Target, ERenderTargetLoadAction::ELoad);

		if (SceneTextures.GBufferB)
		{
			Parameters->RenderTargets[1] = FRenderTargetBinding(SceneTextures.GBufferB, ERenderTargetLoadAction::ELoad);
		}
	}

	Parameters->Common.View = View.ViewUniformBuffer;
	Parameters->Common.LineDataBuffer = GraphBuilder.CreateSRV(LineBuffer, PF_A32B32G32R32F);

	if (!ensureVoxelSlow(View.ViewFrustum.Planes.Num() == 4))
	{
		return;
	}

	const FMatrix Matrix = View.ViewMatrices.GetViewToClip();

	FPlane LeftPlane(ForceInit);
	FPlane RightPlane(ForceInit);
	FPlane TopPlane(ForceInit);
	FPlane BottomPlane(ForceInit);
	ensureVoxelSlow(Matrix.GetFrustumLeftPlane(LeftPlane));
	ensureVoxelSlow(Matrix.GetFrustumRightPlane(RightPlane));
	ensureVoxelSlow(Matrix.GetFrustumTopPlane(TopPlane));
	ensureVoxelSlow(Matrix.GetFrustumBottomPlane(BottomPlane));

	Parameters->Common.FrustumPlanes[0] = FVector4f(FVector4(LeftPlane));
	Parameters->Common.FrustumPlanes[1] = FVector4f(FVector4(RightPlane));
	Parameters->Common.FrustumPlanes[2] = FVector4f(FVector4(TopPlane));
	Parameters->Common.FrustumPlanes[3] = FVector4f(FVector4(BottomPlane));

	const TShaderMapRef<FVoxelDebugLineVS> VertexShader(ShaderMap);
	const TShaderMapRef<FVoxelDebugLinePS> PixelShader(ShaderMap);

	RDG_EVENT_SCOPE_STAT(GraphBuilder, VoxelDebugDrawLines, "VoxelDebugDrawLines");

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("%s", EventName),
		Parameters,
		ERDGPassFlags::Raster,
		[VertexShader, PixelShader, &View, Parameters, NumLines, OverrideDepthStencilState, bWriteAlpha](FRDGAsyncTask, FRHICommandList& RHICmdList)
		{
			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			RHICmdList.SetViewport(
				View.ViewRect.Min.X, View.ViewRect.Min.Y, 0.0f,
				View.ViewRect.Max.X, View.ViewRect.Max.Y, 1.0f);

			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
			GraphicsPSOInit.DepthStencilState = OverrideDepthStencilState
				? OverrideDepthStencilState
				: TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI();
			GraphicsPSOInit.BlendState = bWriteAlpha
				? TStaticBlendStateWriteMask<CW_RGBA>::GetRHI()
				: TStaticBlendStateWriteMask<CW_RGB, CW_RGBA>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;

			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GEmptyVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), Parameters->Common);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *Parameters);

			RHICmdList.DrawIndexedPrimitive(
				GTwoTrianglesIndexBuffer.IndexBufferRHI,
				0,
				0,
				4,
				0,
				2,
				NumLines);
		});
}

static void RenderSpheresInternal(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const TSharedPtr<const TVoxelArray<FVoxelDebugSphere>>& SpheresData,
	TRefCountPtr<FRDGPooledBuffer>& PooledBuffer,
	TWeakPtr<const TVoxelArray<FVoxelDebugSphere>>& UploadedData,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState,
	const TCHAR* EventName,
	bool bWriteAlpha = false)
{
	ensure(IsInRenderingThread());

	const int32 NumSpheres = SpheresData ? SpheresData->Num() : 0;
	if (NumSpheres == 0)
	{
		PooledBuffer = {};
		return;
	}

	VOXEL_FUNCTION_COUNTER_NUM(NumSpheres);

	if (!PooledBuffer ||
		int64(PooledBuffer->Desc.NumElements) < int64(NumSpheres) * 2)
	{
		VOXEL_SCOPE_COUNTER("Create buffer");

		const FRDGBufferRef Buffer = GraphBuilder.CreateBuffer(
			FRDGBufferDesc::CreateBufferDesc(sizeof(FVector4f), NumSpheres * 2),
			TEXT("VoxelDebugDraw.Spheres"));

		PooledBuffer = GraphBuilder.ConvertToExternalBuffer(Buffer);
	}

	const FRDGBufferRef SphereBuffer = GraphBuilder.RegisterExternalBuffer(PooledBuffer);

	if (UploadedData != SpheresData)
	{
		UploadedData = SpheresData;

		FVoxelUtilities::UploadBuffer(
			GraphBuilder,
			SphereBuffer,
			SpheresData->View<uint8>(),
			MakeSharedVoidPtr(SpheresData));
	}

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.FeatureLevel);

	FVoxelDebugSpherePS::FParameters* Parameters = GraphBuilder.AllocParameters<FVoxelDebugSpherePS::FParameters>();

	if (OverrideRenderTargets)
	{
		Parameters->RenderTargets = *OverrideRenderTargets;
	}
	else
	{
		const FSceneTextures& SceneTextures = View.GetSceneTextures();

		Parameters->RenderTargets.DepthStencil = FDepthStencilBinding(
			SceneTextures.Depth.Target,
			ERenderTargetLoadAction::ELoad,
			ERenderTargetLoadAction::ELoad,
			FExclusiveDepthStencil::DepthWrite_StencilWrite);

		Parameters->RenderTargets[0] = FRenderTargetBinding(SceneTextures.Color.Target, ERenderTargetLoadAction::ELoad);

		if (SceneTextures.GBufferB)
		{
			Parameters->RenderTargets[1] = FRenderTargetBinding(SceneTextures.GBufferB, ERenderTargetLoadAction::ELoad);
		}
	}

	Parameters->Common.View = View.ViewUniformBuffer;
	Parameters->Common.SphereDataBuffer = GraphBuilder.CreateSRV(SphereBuffer, PF_A32B32G32R32F);

	if (!ensureVoxelSlow(View.ViewFrustum.Planes.Num() == 4))
	{
		return;
	}

	const FMatrix Matrix = View.ViewMatrices.GetViewToClip();

	FPlane LeftPlane(ForceInit);
	FPlane RightPlane(ForceInit);
	FPlane TopPlane(ForceInit);
	FPlane BottomPlane(ForceInit);
	ensureVoxelSlow(Matrix.GetFrustumLeftPlane(LeftPlane));
	ensureVoxelSlow(Matrix.GetFrustumRightPlane(RightPlane));
	ensureVoxelSlow(Matrix.GetFrustumTopPlane(TopPlane));
	ensureVoxelSlow(Matrix.GetFrustumBottomPlane(BottomPlane));

	Parameters->Common.FrustumPlanes[0] = FVector4f(FVector4(LeftPlane));
	Parameters->Common.FrustumPlanes[1] = FVector4f(FVector4(RightPlane));
	Parameters->Common.FrustumPlanes[2] = FVector4f(FVector4(TopPlane));
	Parameters->Common.FrustumPlanes[3] = FVector4f(FVector4(BottomPlane));

	const TShaderMapRef<FVoxelDebugSphereVS> VertexShader(ShaderMap);
	const TShaderMapRef<FVoxelDebugSpherePS> PixelShader(ShaderMap);

	RDG_EVENT_SCOPE_STAT(GraphBuilder, VoxelDebugDrawSpheres, "VoxelDebugDrawSpheres");

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("%s", EventName),
		Parameters,
		ERDGPassFlags::Raster,
		[VertexShader, PixelShader, &View, Parameters, NumSpheres, OverrideDepthStencilState, bWriteAlpha](FRDGAsyncTask, FRHICommandList& RHICmdList)
		{
			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			RHICmdList.SetViewport(
				View.ViewRect.Min.X, View.ViewRect.Min.Y, 0.0f,
				View.ViewRect.Max.X, View.ViewRect.Max.Y, 1.0f);

			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
			GraphicsPSOInit.DepthStencilState = OverrideDepthStencilState
				? OverrideDepthStencilState
				: TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI();
			GraphicsPSOInit.BlendState = bWriteAlpha
				? TStaticBlendStateWriteMask<CW_RGBA>::GetRHI()
				: TStaticBlendStateWriteMask<CW_RGB, CW_RGBA>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;

			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GEmptyVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), Parameters->Common);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *Parameters);

			RHICmdList.DrawIndexedPrimitive(
				GVoxelDebugSphereIndexBuffer.IndexBufferRHI,
				0,
				0,
				VERTS_PER_SPHERE,
				0,
				QUADS_PER_SPHERE * 2,
				NumSpheres);
		});
}

static void RenderBoxesInternal(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const TSharedPtr<const TVoxelArray<FVoxelDebugBox>>& BoxesData,
	TRefCountPtr<FRDGPooledBuffer>& PooledBuffer,
	TWeakPtr<const TVoxelArray<FVoxelDebugBox>>& UploadedData,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState,
	const TCHAR* EventName,
	bool bWriteAlpha = false)
{
	ensure(IsInRenderingThread());

	const int32 NumBoxes = BoxesData ? BoxesData->Num() : 0;
	if (NumBoxes == 0)
	{
		PooledBuffer = {};
		return;
	}

	VOXEL_FUNCTION_COUNTER_NUM(NumBoxes);

	if (!PooledBuffer ||
		int64(PooledBuffer->Desc.NumElements) < int64(NumBoxes) * 3)
	{
		VOXEL_SCOPE_COUNTER("Create buffer");

		const FRDGBufferRef Buffer = GraphBuilder.CreateBuffer(
			FRDGBufferDesc::CreateBufferDesc(sizeof(FVector4f), NumBoxes * 3),
			TEXT("VoxelDebugDraw.Boxes"));

		PooledBuffer = GraphBuilder.ConvertToExternalBuffer(Buffer);
	}

	const FRDGBufferRef BoxBuffer = GraphBuilder.RegisterExternalBuffer(PooledBuffer);

	if (UploadedData != BoxesData)
	{
		UploadedData = BoxesData;

		FVoxelUtilities::UploadBuffer(
			GraphBuilder,
			BoxBuffer,
			BoxesData->View<uint8>(),
			MakeSharedVoidPtr(BoxesData));
	}

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.FeatureLevel);

	FVoxelDebugBoxPS::FParameters* Parameters = GraphBuilder.AllocParameters<FVoxelDebugBoxPS::FParameters>();

	if (OverrideRenderTargets)
	{
		Parameters->RenderTargets = *OverrideRenderTargets;
	}
	else
	{
		const FSceneTextures& SceneTextures = View.GetSceneTextures();

		Parameters->RenderTargets.DepthStencil = FDepthStencilBinding(
			SceneTextures.Depth.Target,
			ERenderTargetLoadAction::ELoad,
			ERenderTargetLoadAction::ELoad,
			FExclusiveDepthStencil::DepthWrite_StencilWrite);

		Parameters->RenderTargets[0] = FRenderTargetBinding(SceneTextures.Color.Target, ERenderTargetLoadAction::ELoad);

		if (SceneTextures.GBufferB)
		{
			Parameters->RenderTargets[1] = FRenderTargetBinding(SceneTextures.GBufferB, ERenderTargetLoadAction::ELoad);
		}
	}

	Parameters->Common.View = View.ViewUniformBuffer;
	Parameters->Common.BoxDataBuffer = GraphBuilder.CreateSRV(BoxBuffer, PF_A32B32G32R32F);

	if (!ensureVoxelSlow(View.ViewFrustum.Planes.Num() == 4))
	{
		return;
	}

	const FMatrix Matrix = View.ViewMatrices.GetViewToClip();

	FPlane LeftPlane(ForceInit);
	FPlane RightPlane(ForceInit);
	FPlane TopPlane(ForceInit);
	FPlane BottomPlane(ForceInit);
	ensureVoxelSlow(Matrix.GetFrustumLeftPlane(LeftPlane));
	ensureVoxelSlow(Matrix.GetFrustumRightPlane(RightPlane));
	ensureVoxelSlow(Matrix.GetFrustumTopPlane(TopPlane));
	ensureVoxelSlow(Matrix.GetFrustumBottomPlane(BottomPlane));

	Parameters->Common.FrustumPlanes[0] = FVector4f(FVector4(LeftPlane));
	Parameters->Common.FrustumPlanes[1] = FVector4f(FVector4(RightPlane));
	Parameters->Common.FrustumPlanes[2] = FVector4f(FVector4(TopPlane));
	Parameters->Common.FrustumPlanes[3] = FVector4f(FVector4(BottomPlane));

	const TShaderMapRef<FVoxelDebugBoxVS> VertexShader(ShaderMap);
	const TShaderMapRef<FVoxelDebugBoxPS> PixelShader(ShaderMap);

	RDG_EVENT_SCOPE_STAT(GraphBuilder, VoxelDebugDrawBoxes, "VoxelDebugDrawBoxes");

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("%s", EventName),
		Parameters,
		ERDGPassFlags::Raster,
		[VertexShader, PixelShader, &View, Parameters, NumBoxes, OverrideDepthStencilState, bWriteAlpha](FRDGAsyncTask, FRHICommandList& RHICmdList)
		{
			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			RHICmdList.SetViewport(
				View.ViewRect.Min.X, View.ViewRect.Min.Y, 0.0f,
				View.ViewRect.Max.X, View.ViewRect.Max.Y, 1.0f);

			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
			GraphicsPSOInit.DepthStencilState = OverrideDepthStencilState
				? OverrideDepthStencilState
				: TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI();
			GraphicsPSOInit.BlendState = bWriteAlpha
				? TStaticBlendStateWriteMask<CW_RGBA>::GetRHI()
				: TStaticBlendStateWriteMask<CW_RGB, CW_RGBA>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;

			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GEmptyVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), Parameters->Common);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *Parameters);

			RHICmdList.DrawIndexedPrimitive(
				GVoxelDebugBoxIndexBuffer.IndexBufferRHI,
				0,
				0,
				VERTS_PER_BOX,
				0,
				QUADS_PER_BOX * 2,
				NumBoxes);
		});
}

static void RenderSphericalSectorsInternal(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const TSharedPtr<const TVoxelArray<FVoxelDebugSphericalSector>>& SectorsData,
	TRefCountPtr<FRDGPooledBuffer>& PooledBuffer,
	TWeakPtr<const TVoxelArray<FVoxelDebugSphericalSector>>& UploadedData,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState,
	const TCHAR* EventName,
	bool bWriteAlpha = false)
{
	ensure(IsInRenderingThread());

	const int32 NumSectors = SectorsData ? SectorsData->Num() : 0;
	if (NumSectors == 0)
	{
		PooledBuffer = {};
		return;
	}

	VOXEL_FUNCTION_COUNTER_NUM(NumSectors);

	if (!PooledBuffer ||
		int64(PooledBuffer->Desc.NumElements) < int64(NumSectors) * 3)
	{
		VOXEL_SCOPE_COUNTER("Create buffer");

		const FRDGBufferRef Buffer = GraphBuilder.CreateBuffer(
			FRDGBufferDesc::CreateBufferDesc(sizeof(FVector4f), NumSectors * 3),
			TEXT("VoxelDebugDraw.SphericalSectors"));

		PooledBuffer = GraphBuilder.ConvertToExternalBuffer(Buffer);
	}

	const FRDGBufferRef SectorBuffer = GraphBuilder.RegisterExternalBuffer(PooledBuffer);

	if (UploadedData != SectorsData)
	{
		UploadedData = SectorsData;

		FVoxelUtilities::UploadBuffer(
			GraphBuilder,
			SectorBuffer,
			SectorsData->View<uint8>(),
			MakeSharedVoidPtr(SectorsData));
	}

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.FeatureLevel);

	FVoxelDebugSphericalSectorPS::FParameters* Parameters = GraphBuilder.AllocParameters<FVoxelDebugSphericalSectorPS::FParameters>();

	if (OverrideRenderTargets)
	{
		Parameters->RenderTargets = *OverrideRenderTargets;
	}
	else
	{
		const FSceneTextures& SceneTextures = View.GetSceneTextures();

		Parameters->RenderTargets.DepthStencil = FDepthStencilBinding(
			SceneTextures.Depth.Target,
			ERenderTargetLoadAction::ELoad,
			ERenderTargetLoadAction::ELoad,
			FExclusiveDepthStencil::DepthWrite_StencilWrite);

		Parameters->RenderTargets[0] = FRenderTargetBinding(SceneTextures.Color.Target, ERenderTargetLoadAction::ELoad);

		if (SceneTextures.GBufferB)
		{
			Parameters->RenderTargets[1] = FRenderTargetBinding(SceneTextures.GBufferB, ERenderTargetLoadAction::ELoad);
		}
	}

	Parameters->Common.View = View.ViewUniformBuffer;
	Parameters->Common.SectorDataBuffer = GraphBuilder.CreateSRV(SectorBuffer, PF_A32B32G32R32F);

	if (!ensureVoxelSlow(View.ViewFrustum.Planes.Num() == 4))
	{
		return;
	}

	const FMatrix Matrix = View.ViewMatrices.GetViewToClip();

	FPlane LeftPlane(ForceInit);
	FPlane RightPlane(ForceInit);
	FPlane TopPlane(ForceInit);
	FPlane BottomPlane(ForceInit);
	ensureVoxelSlow(Matrix.GetFrustumLeftPlane(LeftPlane));
	ensureVoxelSlow(Matrix.GetFrustumRightPlane(RightPlane));
	ensureVoxelSlow(Matrix.GetFrustumTopPlane(TopPlane));
	ensureVoxelSlow(Matrix.GetFrustumBottomPlane(BottomPlane));

	Parameters->Common.FrustumPlanes[0] = FVector4f(FVector4(LeftPlane));
	Parameters->Common.FrustumPlanes[1] = FVector4f(FVector4(RightPlane));
	Parameters->Common.FrustumPlanes[2] = FVector4f(FVector4(TopPlane));
	Parameters->Common.FrustumPlanes[3] = FVector4f(FVector4(BottomPlane));

	const TShaderMapRef<FVoxelDebugSphericalSectorVS> VertexShader(ShaderMap);
	const TShaderMapRef<FVoxelDebugSphericalSectorPS> PixelShader(ShaderMap);

	RDG_EVENT_SCOPE_STAT(GraphBuilder, VoxelDebugDrawSphericalSectors, "VoxelDebugDrawSphericalSectors");

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("%s", EventName),
		Parameters,
		ERDGPassFlags::Raster,
		[VertexShader, PixelShader, &View, Parameters, NumSectors, OverrideDepthStencilState, bWriteAlpha](FRDGAsyncTask, FRHICommandList& RHICmdList)
		{
			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			RHICmdList.SetViewport(
				View.ViewRect.Min.X, View.ViewRect.Min.Y, 0.0f,
				View.ViewRect.Max.X, View.ViewRect.Max.Y, 1.0f);

			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
			GraphicsPSOInit.DepthStencilState = OverrideDepthStencilState
				? OverrideDepthStencilState
				: TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI();
			GraphicsPSOInit.BlendState = bWriteAlpha
				? TStaticBlendStateWriteMask<CW_RGBA>::GetRHI()
				: TStaticBlendStateWriteMask<CW_RGB, CW_RGBA>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;

			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GEmptyVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), Parameters->Common);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), *Parameters);

			RHICmdList.DrawIndexedPrimitive(
				GVoxelDebugSphericalSectorIndexBuffer.IndexBufferRHI,
				0,
				0,
				VERTS_PER_SECTOR,
				0,
				QUADS_PER_SECTOR * 2,
				NumSectors);
		});
}

void FVoxelDebugDrawerWorldManager::RenderPoints_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState)
{
	RenderPointsInternal(GraphBuilder, View, StaticBuffers.PointsToRender_RenderThread, StaticBuffers.PooledPointBuffer, StaticBuffers.UploadedPointsToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Static Points"));
	RenderPointsInternal(GraphBuilder, View, DynamicBuffers.PointsToRender_RenderThread, DynamicBuffers.PooledPointBuffer, DynamicBuffers.UploadedPointsToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Dynamic Points"));
}

void FVoxelDebugDrawerWorldManager::RenderLines_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState)
{
	RenderLinesInternal(GraphBuilder, View, StaticBuffers.LinesToRender_RenderThread, StaticBuffers.PooledLineBuffer, StaticBuffers.UploadedLinesToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Static Lines"));
	RenderLinesInternal(GraphBuilder, View, DynamicBuffers.LinesToRender_RenderThread, DynamicBuffers.PooledLineBuffer, DynamicBuffers.UploadedLinesToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Dynamic Lines"));
}

void FVoxelDebugDrawerWorldManager::RenderForegroundPoints_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots& RenderTargets,
	FRHIDepthStencilState* DepthStencilState)
{
	RenderPointsInternal(GraphBuilder, View, StaticBuffers.ForegroundPointsToRender_RenderThread, StaticBuffers.PooledForegroundPointBuffer, StaticBuffers.UploadedForegroundPointsToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Static Foreground Points"));
	RenderPointsInternal(GraphBuilder, View, DynamicBuffers.ForegroundPointsToRender_RenderThread, DynamicBuffers.PooledForegroundPointBuffer, DynamicBuffers.UploadedForegroundPointsToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Dynamic Foreground Points"));
}

void FVoxelDebugDrawerWorldManager::RenderForegroundLines_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots& RenderTargets,
	FRHIDepthStencilState* DepthStencilState)
{
	RenderLinesInternal(GraphBuilder, View, StaticBuffers.ForegroundLinesToRender_RenderThread, StaticBuffers.PooledForegroundLineBuffer, StaticBuffers.UploadedForegroundLinesToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Static Foreground Lines"));
	RenderLinesInternal(GraphBuilder, View, DynamicBuffers.ForegroundLinesToRender_RenderThread, DynamicBuffers.PooledForegroundLineBuffer, DynamicBuffers.UploadedForegroundLinesToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Dynamic Foreground Lines"));
}

void FVoxelDebugDrawerWorldManager::RenderBoxes_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState)
{
	RenderBoxesInternal(GraphBuilder, View, StaticBuffers.BoxesToRender_RenderThread, StaticBuffers.PooledBoxBuffer, StaticBuffers.UploadedBoxesToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Static Boxes"));
	RenderBoxesInternal(GraphBuilder, View, DynamicBuffers.BoxesToRender_RenderThread, DynamicBuffers.PooledBoxBuffer, DynamicBuffers.UploadedBoxesToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Dynamic Boxes"));
}

void FVoxelDebugDrawerWorldManager::RenderForegroundBoxes_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots& RenderTargets,
	FRHIDepthStencilState* DepthStencilState)
{
	RenderBoxesInternal(GraphBuilder, View, StaticBuffers.ForegroundBoxesToRender_RenderThread, StaticBuffers.PooledForegroundBoxBuffer, StaticBuffers.UploadedForegroundBoxesToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Static Foreground Boxes"));
	RenderBoxesInternal(GraphBuilder, View, DynamicBuffers.ForegroundBoxesToRender_RenderThread, DynamicBuffers.PooledForegroundBoxBuffer, DynamicBuffers.UploadedForegroundBoxesToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Dynamic Foreground Boxes"));
}

void FVoxelDebugDrawerWorldManager::RenderSphericalSectors_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState)
{
	RenderSphericalSectorsInternal(GraphBuilder, View, StaticBuffers.SphericalSectorsToRender_RenderThread, StaticBuffers.PooledSphericalSectorBuffer, StaticBuffers.UploadedSphericalSectorsToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Static SphericalSectors"));
	RenderSphericalSectorsInternal(GraphBuilder, View, DynamicBuffers.SphericalSectorsToRender_RenderThread, DynamicBuffers.PooledSphericalSectorBuffer, DynamicBuffers.UploadedSphericalSectorsToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Dynamic SphericalSectors"));
}

void FVoxelDebugDrawerWorldManager::RenderSpheres_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots* OverrideRenderTargets,
	FRHIDepthStencilState* OverrideDepthStencilState)
{
	RenderSpheresInternal(GraphBuilder, View, StaticBuffers.SpheresToRender_RenderThread, StaticBuffers.PooledSphereBuffer, StaticBuffers.UploadedSpheresToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Static Spheres"));
	RenderSpheresInternal(GraphBuilder, View, DynamicBuffers.SpheresToRender_RenderThread, DynamicBuffers.PooledSphereBuffer, DynamicBuffers.UploadedSpheresToRender, OverrideRenderTargets, OverrideDepthStencilState, TEXT("VoxelDebugDraw Dynamic Spheres"));
}

void FVoxelDebugDrawerWorldManager::RenderForegroundSphericalSectors_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots& RenderTargets,
	FRHIDepthStencilState* DepthStencilState)
{
	RenderSphericalSectorsInternal(GraphBuilder, View, StaticBuffers.ForegroundSphericalSectorsToRender_RenderThread, StaticBuffers.PooledForegroundSphericalSectorBuffer, StaticBuffers.UploadedForegroundSphericalSectorsToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Static Foreground SphericalSectors"));
	RenderSphericalSectorsInternal(GraphBuilder, View, DynamicBuffers.ForegroundSphericalSectorsToRender_RenderThread, DynamicBuffers.PooledForegroundSphericalSectorBuffer, DynamicBuffers.UploadedForegroundSphericalSectorsToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Dynamic Foreground SphericalSectors"));
}

void FVoxelDebugDrawerWorldManager::RenderForegroundSpheres_RenderThread(
	FRDGBuilder& GraphBuilder,
	FViewInfo& View,
	const FRenderTargetBindingSlots& RenderTargets,
	FRHIDepthStencilState* DepthStencilState)
{
	RenderSpheresInternal(GraphBuilder, View, StaticBuffers.ForegroundSpheresToRender_RenderThread, StaticBuffers.PooledForegroundSphereBuffer, StaticBuffers.UploadedForegroundSpheresToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Static Foreground Spheres"));
	RenderSpheresInternal(GraphBuilder, View, DynamicBuffers.ForegroundSpheresToRender_RenderThread, DynamicBuffers.PooledForegroundSphereBuffer, DynamicBuffers.UploadedForegroundSpheresToRender, &RenderTargets, DepthStencilState, TEXT("VoxelDebugDraw Dynamic Foreground Spheres"));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void FlattenDraws(
	const TVoxelArray<TSharedPtr<const FVoxelDebugDraw>>& Draws,
	TVoxelArray<FVoxelDebugPoint>& OutPoints,
	TVoxelArray<FVoxelDebugLine>& OutLines,
	TVoxelArray<FVoxelDebugSphere>& OutSpheres,
	TVoxelArray<FVoxelDebugSphericalSector>& OutSphericalSectors,
	TVoxelArray<FVoxelDebugBox>& OutBoxes,
	TVoxelArray<FVoxelDebugPoint>& OutForegroundPoints,
	TVoxelArray<FVoxelDebugLine>& OutForegroundLines,
	TVoxelArray<FVoxelDebugSphere>& OutForegroundSpheres,
	TVoxelArray<FVoxelDebugSphericalSector>& OutForegroundSphericalSectors,
	TVoxelArray<FVoxelDebugBox>& OutForegroundBoxes)
{
	VOXEL_FUNCTION_COUNTER();

	int64 NumPoints = 0;
	int64 NumLines = 0;
	int64 NumSpheres = 0;
	int64 NumSphericalSectors = 0;
	int64 NumBoxes = 0;
	int64 NumForegroundPoints = 0;
	int64 NumForegroundLines = 0;
	int64 NumForegroundSpheres = 0;
	int64 NumForegroundSphericalSectors = 0;
	int64 NumForegroundBoxes = 0;

	for (const TSharedPtr<const FVoxelDebugDraw>& Draw : Draws)
	{
		NumPoints += Draw->Points.Num();
		NumLines += Draw->Lines.Num();
		NumSpheres += Draw->Spheres.Num();
		NumSphericalSectors += Draw->SphericalSectors.Num();
		NumBoxes += Draw->Boxes.Num();
		NumForegroundPoints += Draw->ForegroundPoints.Num();
		NumForegroundLines += Draw->ForegroundLines.Num();
		NumForegroundSpheres += Draw->ForegroundSpheres.Num();
		NumForegroundSphericalSectors += Draw->ForegroundSphericalSectors.Num();
		NumForegroundBoxes += Draw->ForegroundBoxes.Num();
	}

	FVoxelUtilities::SetNumFast(OutPoints, NumPoints);
	FVoxelUtilities::SetNumFast(OutLines, NumLines);
	FVoxelUtilities::SetNumFast(OutSpheres, NumSpheres);
	FVoxelUtilities::SetNumFast(OutSphericalSectors, NumSphericalSectors);
	FVoxelUtilities::SetNumFast(OutBoxes, NumBoxes);
	FVoxelUtilities::SetNumFast(OutForegroundPoints, NumForegroundPoints);
	FVoxelUtilities::SetNumFast(OutForegroundLines, NumForegroundLines);
	FVoxelUtilities::SetNumFast(OutForegroundSpheres, NumForegroundSpheres);
	FVoxelUtilities::SetNumFast(OutForegroundSphericalSectors, NumForegroundSphericalSectors);
	FVoxelUtilities::SetNumFast(OutForegroundBoxes, NumForegroundBoxes);

	int32 PointIndex = 0;
	int32 LineIndex = 0;
	int32 SphereIndex = 0;
	int32 SphericalSectorIndex = 0;
	int32 BoxIndex = 0;
	int32 ForegroundPointIndex = 0;
	int32 ForegroundLineIndex = 0;
	int32 ForegroundSphereIndex = 0;
	int32 ForegroundSphericalSectorIndex = 0;
	int32 ForegroundBoxIndex = 0;

	for (const TSharedPtr<const FVoxelDebugDraw>& Draw : Draws)
	{
		if (Draw->Points.Num() > 0)
		{
			Draw->Points.CopyTo(OutPoints.View().Slice(PointIndex, Draw->Points.Num()));
			PointIndex += Draw->Points.Num();
		}

		if (Draw->Lines.Num() > 0)
		{
			Draw->Lines.CopyTo(OutLines.View().Slice(LineIndex, Draw->Lines.Num()));
			LineIndex += Draw->Lines.Num();
		}

		if (Draw->Spheres.Num() > 0)
		{
			Draw->Spheres.CopyTo(OutSpheres.View().Slice(SphereIndex, Draw->Spheres.Num()));
			SphereIndex += Draw->Spheres.Num();
		}

		if (Draw->SphericalSectors.Num() > 0)
		{
			Draw->SphericalSectors.CopyTo(OutSphericalSectors.View().Slice(SphericalSectorIndex, Draw->SphericalSectors.Num()));
			SphericalSectorIndex += Draw->SphericalSectors.Num();
		}

		if (Draw->Boxes.Num() > 0)
		{
			Draw->Boxes.CopyTo(OutBoxes.View().Slice(BoxIndex, Draw->Boxes.Num()));
			BoxIndex += Draw->Boxes.Num();
		}

		if (Draw->ForegroundPoints.Num() > 0)
		{
			Draw->ForegroundPoints.CopyTo(OutForegroundPoints.View().Slice(ForegroundPointIndex, Draw->ForegroundPoints.Num()));
			ForegroundPointIndex += Draw->ForegroundPoints.Num();
		}

		if (Draw->ForegroundLines.Num() > 0)
		{
			Draw->ForegroundLines.CopyTo(OutForegroundLines.View().Slice(ForegroundLineIndex, Draw->ForegroundLines.Num()));
			ForegroundLineIndex += Draw->ForegroundLines.Num();
		}

		if (Draw->ForegroundSpheres.Num() > 0)
		{
			Draw->ForegroundSpheres.CopyTo(OutForegroundSpheres.View().Slice(ForegroundSphereIndex, Draw->ForegroundSpheres.Num()));
			ForegroundSphereIndex += Draw->ForegroundSpheres.Num();
		}

		if (Draw->ForegroundSphericalSectors.Num() > 0)
		{
			Draw->ForegroundSphericalSectors.CopyTo(OutForegroundSphericalSectors.View().Slice(ForegroundSphericalSectorIndex, Draw->ForegroundSphericalSectors.Num()));
			ForegroundSphericalSectorIndex += Draw->ForegroundSphericalSectors.Num();
		}

		if (Draw->ForegroundBoxes.Num() > 0)
		{
			Draw->ForegroundBoxes.CopyTo(OutForegroundBoxes.View().Slice(ForegroundBoxIndex, Draw->ForegroundBoxes.Num()));
			ForegroundBoxIndex += Draw->ForegroundBoxes.Num();
		}
	}
}

void FVoxelDebugDrawerWorldManager::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	World_Unsafe = GetWorld().Resolve_Ensured();
}

void FVoxelDebugDrawerWorldManager::KickFlatten(const uint32 FrameNumber)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	if (LastKickFrameNumber == FrameNumber)
	{
		return;
	}

	LastKickFrameNumber = FrameNumber;

	FPendingFrame& Frame = PendingFrames[FrameNumber % NumPendingFrames];

	// Wait for this slot's previous use to complete (should always be done — it's N frames old)
	if (!Frame.Flatten.IsComplete())
	{
		VOXEL_SCOPE_COUNTER("WaitForSlotReuse");

		while (!Frame.Flatten.IsComplete())
		{
			FPlatformProcess::YieldThread();
		}
	}

	Frame.bReady.store(false, std::memory_order_release);
	Frame.bApplied = false;
	Frame.BufferData.Reset();

	// Collect draws from all groups on the game thread (needs lock, but fast)
	TVoxelArray<TSharedPtr<const FVoxelDebugDraw>> StaticDraws;
	TVoxelArray<TSharedPtr<const FVoxelDebugDraw>> DynamicDraws;
	uint64 CurrentStaticGeneration = 0;

	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		const double Time = FPlatformTime::Seconds();

		for (auto It = Groups_RequiresLock.CreateIterator(); It; ++It)
		{
			const TSharedPtr<FVoxelDebugDrawGroup> Group = It->Pin();
			if (!Group)
			{
				It.RemoveCurrent();
				continue;
			}

			Group->IterateDraws(Time, StaticDraws, DynamicDraws);
			CurrentStaticGeneration += Group->StaticGeneration.load(std::memory_order_relaxed);
		}
	}

	const bool bStaticChanged = CurrentStaticGeneration != LastStaticGeneration;
	LastStaticGeneration = CurrentStaticGeneration;

	// Kick async flatten on background thread
	Frame.Flatten = Voxel::AsyncTask([&Frame, bStaticChanged,
		StaticDraws = MoveTemp(StaticDraws),
		DynamicDraws = MoveTemp(DynamicDraws)]
	{
		TSharedPtr<FPendingBufferData> Result = MakeShared<FPendingBufferData>();
		Result->bStaticChanged = bStaticChanged;

		FlattenDraws(DynamicDraws,
			Result->DynPoints, Result->DynLines, Result->DynSpheres, Result->DynSectors, Result->DynBoxes,
			Result->DynFgPoints, Result->DynFgLines, Result->DynFgSpheres, Result->DynFgSectors, Result->DynFgBoxes);

		if (bStaticChanged)
		{
			Result->StatPoints = MakeShared<TVoxelArray<FVoxelDebugPoint>>();
			Result->StatLines = MakeShared<TVoxelArray<FVoxelDebugLine>>();
			Result->StatSpheres = MakeShared<TVoxelArray<FVoxelDebugSphere>>();
			Result->StatSectors = MakeShared<TVoxelArray<FVoxelDebugSphericalSector>>();
			Result->StatBoxes = MakeShared<TVoxelArray<FVoxelDebugBox>>();
			Result->StatFgPoints = MakeShared<TVoxelArray<FVoxelDebugPoint>>();
			Result->StatFgLines = MakeShared<TVoxelArray<FVoxelDebugLine>>();
			Result->StatFgSpheres = MakeShared<TVoxelArray<FVoxelDebugSphere>>();
			Result->StatFgSectors = MakeShared<TVoxelArray<FVoxelDebugSphericalSector>>();
			Result->StatFgBoxes = MakeShared<TVoxelArray<FVoxelDebugBox>>();
			FlattenDraws(StaticDraws,
				*Result->StatPoints, *Result->StatLines, *Result->StatSpheres, *Result->StatSectors, *Result->StatBoxes,
				*Result->StatFgPoints, *Result->StatFgLines, *Result->StatFgSpheres, *Result->StatFgSectors, *Result->StatFgBoxes);
		}

		Frame.BufferData = Result;
		Frame.bReady.store(true, std::memory_order_release);
	});
}

void FVoxelDebugDrawerWorldManager::WaitForFlattenAndApply(const uint32 FrameNumber)
{
	VOXEL_FUNCTION_COUNTER();

	FPendingFrame& Frame = PendingFrames[FrameNumber % NumPendingFrames];

	if (Frame.bApplied)
	{
		return;
	}

	if (!Frame.bReady.load(std::memory_order_acquire))
	{
		VOXEL_SCOPE_COUNTER("WaitForFlatten");

		while (!Frame.bReady.load(std::memory_order_acquire))
		{
			FPlatformProcess::YieldThread();
		}
	}

	Frame.bApplied = true;

	FPendingBufferData* Result = Frame.BufferData.Get();
	if (!Result)
	{
		return;
	}

	if (Result->bStaticChanged)
	{
		StaticBuffers.PointsToRender_RenderThread = Result->StatPoints;
		StaticBuffers.LinesToRender_RenderThread = Result->StatLines;
		StaticBuffers.SpheresToRender_RenderThread = Result->StatSpheres;
		StaticBuffers.SphericalSectorsToRender_RenderThread = Result->StatSectors;
		StaticBuffers.BoxesToRender_RenderThread = Result->StatBoxes;
		StaticBuffers.ForegroundPointsToRender_RenderThread = Result->StatFgPoints;
		StaticBuffers.ForegroundLinesToRender_RenderThread = Result->StatFgLines;
		StaticBuffers.ForegroundSpheresToRender_RenderThread = Result->StatFgSpheres;
		StaticBuffers.ForegroundSphericalSectorsToRender_RenderThread = Result->StatFgSectors;
		StaticBuffers.ForegroundBoxesToRender_RenderThread = Result->StatFgBoxes;
	}

	DynamicBuffers.PointsToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynPoints));
	DynamicBuffers.LinesToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynLines));
	DynamicBuffers.SpheresToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynSpheres));
	DynamicBuffers.SphericalSectorsToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynSectors));
	DynamicBuffers.BoxesToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynBoxes));
	DynamicBuffers.ForegroundPointsToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynFgPoints));
	DynamicBuffers.ForegroundLinesToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynFgLines));
	DynamicBuffers.ForegroundSpheresToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynFgSpheres));
	DynamicBuffers.ForegroundSphericalSectorsToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynFgSectors));
	DynamicBuffers.ForegroundBoxesToRender_RenderThread = MakeSharedCopy(MoveTemp(Result->DynFgBoxes));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDebugDrawerManager::Tick()
{
	DefaultWorld = GWorld.GetReference();
}

void FVoxelDebugDrawerManager::BeginRenderViewFamily(FSceneViewFamily& ViewFamily)
{
	VOXEL_FUNCTION_COUNTER();

	const uint32 FrameNumber = ViewFamily.FrameNumber;

	for (const TSharedRef<FVoxelDebugDrawerWorldManager>& Manager : FVoxelDebugDrawerWorldManager::GetAll())
	{
		Manager->KickFlatten(FrameNumber);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDebugDrawerManager::PostRenderBasePassDeferred_RenderThread(
	FRDGBuilder& GraphBuilder,
	FSceneView& View,
	const FRenderTargetBindingSlots& RenderTargets,
	const TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTextures)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInRenderingThread());

	if (!ensureVoxelSlow(View.bIsViewInfo))
	{
		return;
	}

	// Skip base pass rendering when editor compositing is active — draws will go through the editor compositing hooks instead
	if (View.Family->EngineShowFlags.CompositeEditorPrimitives)
	{
		return;
	}

	const UWorld* World_Unsafe = View.Family->Scene->GetWorld();

	for (const TSharedRef<FVoxelDebugDrawerWorldManager>& Manager : FVoxelDebugDrawerWorldManager::GetAll())
	{
		if (Manager->World_Unsafe == World_Unsafe)
		{
			Manager->WaitForFlattenAndApply(View.Family->FrameNumber);
			Manager->RenderPoints_RenderThread(GraphBuilder, static_cast<FViewInfo&>(View));
			Manager->RenderLines_RenderThread(GraphBuilder, static_cast<FViewInfo&>(View));
			Manager->RenderSpheres_RenderThread(GraphBuilder, static_cast<FViewInfo&>(View));
			Manager->RenderSphericalSectors_RenderThread(GraphBuilder, static_cast<FViewInfo&>(View));
			Manager->RenderBoxes_RenderThread(GraphBuilder, static_cast<FViewInfo&>(View));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** Upload a buffer if the data changed, create an RDG SRV, and put it in external access mode. */
template<typename T>
static void PrepareBuffer(
	FRDGBuilder& GraphBuilder,
	const TSharedPtr<const TVoxelArray<T>>& Data,
	TRefCountPtr<FRDGPooledBuffer>& PooledBuffer,
	TWeakPtr<const TVoxelArray<T>>& UploadedData,
	FRDGBufferSRVRef& OutSRV,
	FSceneViewExtensionBase::FEditorPrimitiveBufferBindings& Bindings,
	const TCHAR* DebugName)
{
	const int32 NumElements = Data ? Data->Num() : 0;
	if (NumElements == 0)
	{
		PooledBuffer = {};
		OutSRV = nullptr;
		return;
	}

	const int32 NumFloat4s = NumElements * sizeof(T) / sizeof(FVector4f);

	if (!PooledBuffer ||
		int64(PooledBuffer->Desc.NumElements) < NumFloat4s)
	{
		const FRDGBufferRef Buffer = GraphBuilder.CreateBuffer(
			FRDGBufferDesc::CreateBufferDesc(sizeof(FVector4f), NumFloat4s),
			DebugName);

		PooledBuffer = GraphBuilder.ConvertToExternalBuffer(Buffer);
	}

	const FRDGBufferRef Buffer = GraphBuilder.RegisterExternalBuffer(PooledBuffer);

	if (UploadedData != Data)
	{
		UploadedData = Data;

		FVoxelUtilities::UploadBuffer(
			GraphBuilder,
			Buffer,
			Data->template View<uint8>(),
			MakeSharedVoidPtr(Data));
	}

	OutSRV = GraphBuilder.CreateSRV(Buffer, PF_A32B32G32R32F);

	// Register SRV on the pass parameters so RDG resolves its RHI resource
	Bindings.BufferAccesses.Emplace(Buffer, ERHIAccess::SRVGraphics);
	Bindings.AddSRV(OutSRV);
}

static void PrepareBufferSet(FRDGBuilder& GraphBuilder, FVoxelDebugDrawerWorldManager::FBufferSet& Buffers, FSceneViewExtensionBase::FEditorPrimitiveBufferBindings& Bindings)
{
	PrepareBuffer(GraphBuilder, Buffers.PointsToRender_RenderThread, Buffers.PooledPointBuffer, Buffers.UploadedPointsToRender, Buffers.PointBufferSRV, Bindings, TEXT("VoxelDebugDraw.Points"));
	PrepareBuffer(GraphBuilder, Buffers.LinesToRender_RenderThread, Buffers.PooledLineBuffer, Buffers.UploadedLinesToRender, Buffers.LineBufferSRV, Bindings, TEXT("VoxelDebugDraw.Lines"));
	PrepareBuffer(GraphBuilder, Buffers.SpheresToRender_RenderThread, Buffers.PooledSphereBuffer, Buffers.UploadedSpheresToRender, Buffers.SphereBufferSRV, Bindings, TEXT("VoxelDebugDraw.Spheres"));
	PrepareBuffer(GraphBuilder, Buffers.SphericalSectorsToRender_RenderThread, Buffers.PooledSphericalSectorBuffer, Buffers.UploadedSphericalSectorsToRender, Buffers.SphericalSectorBufferSRV, Bindings, TEXT("VoxelDebugDraw.SphericalSectors"));
	PrepareBuffer(GraphBuilder, Buffers.BoxesToRender_RenderThread, Buffers.PooledBoxBuffer, Buffers.UploadedBoxesToRender, Buffers.BoxBufferSRV, Bindings, TEXT("VoxelDebugDraw.Boxes"));
	PrepareBuffer(GraphBuilder, Buffers.ForegroundPointsToRender_RenderThread, Buffers.PooledForegroundPointBuffer, Buffers.UploadedForegroundPointsToRender, Buffers.ForegroundPointBufferSRV, Bindings, TEXT("VoxelDebugDraw.ForegroundPoints"));
	PrepareBuffer(GraphBuilder, Buffers.ForegroundLinesToRender_RenderThread, Buffers.PooledForegroundLineBuffer, Buffers.UploadedForegroundLinesToRender, Buffers.ForegroundLineBufferSRV, Bindings, TEXT("VoxelDebugDraw.ForegroundLines"));
	PrepareBuffer(GraphBuilder, Buffers.ForegroundSpheresToRender_RenderThread, Buffers.PooledForegroundSphereBuffer, Buffers.UploadedForegroundSpheresToRender, Buffers.ForegroundSphereBufferSRV, Bindings, TEXT("VoxelDebugDraw.ForegroundSpheres"));
	PrepareBuffer(GraphBuilder, Buffers.ForegroundSphericalSectorsToRender_RenderThread, Buffers.PooledForegroundSphericalSectorBuffer, Buffers.UploadedForegroundSphericalSectorsToRender, Buffers.ForegroundSphericalSectorBufferSRV, Bindings, TEXT("VoxelDebugDraw.ForegroundSphericalSectors"));
	PrepareBuffer(GraphBuilder, Buffers.ForegroundBoxesToRender_RenderThread, Buffers.PooledForegroundBoxBuffer, Buffers.UploadedForegroundBoxesToRender, Buffers.ForegroundBoxBufferSRV, Bindings, TEXT("VoxelDebugDraw.ForegroundBoxes"));
}

void FVoxelDebugDrawerManager::PrepareBuffersForWorld(FRDGBuilder& GraphBuilder, const FSceneView& View, FSceneViewExtensionBase::FEditorPrimitiveBufferBindings& Bindings)
{
	if (!View.bIsViewInfo)
	{
		return;
	}

	const UWorld* World_Unsafe = View.Family->Scene->GetWorld();

	for (const TSharedRef<FVoxelDebugDrawerWorldManager>& Manager : FVoxelDebugDrawerWorldManager::GetAll())
	{
		if (Manager->World_Unsafe != World_Unsafe)
		{
			continue;
		}

		Manager->WaitForFlattenAndApply(View.Family->FrameNumber);
		PrepareBufferSet(GraphBuilder, Manager->StaticBuffers, Bindings);
		PrepareBufferSet(GraphBuilder, Manager->DynamicBuffers, Bindings);
	}
}

/** Draw points or lines using a prepared RDG SRV, inside the engine's pass. */
template<typename VSType, typename PSType>
static void DrawWithSRV(
	FRHICommandList& RHICmdList,
	const FViewInfo& View,
	FRDGBufferSRVRef SRV,
	int32 NumInstances,
	FRHIDepthStencilState* DepthStencilState)
{
	if (!SRV || NumInstances == 0)
	{
		return;
	}

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.FeatureLevel);
	const TShaderMapRef<VSType> VertexShader(ShaderMap);
	const TShaderMapRef<PSType> PixelShader(ShaderMap);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	GraphicsPSOInit.DepthStencilState = DepthStencilState;

	if constexpr (std::is_same_v<VSType, FVoxelDebugPointVS>)
	{
		GraphicsPSOInit.BlendState = TStaticBlendState<
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				CW_RGBA, BO_Add, BF_One, BF_Zero, BO_Add, BF_One, BF_Zero,
				true>::GetRHI();
	}
	else
	{
		GraphicsPSOInit.BlendState = TStaticBlendStateWriteMask<CW_RGBA>::GetRHI();
	}
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GEmptyVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

	auto SetFrustumPlanes = [&](auto& OutPlanes)
	{
		const FMatrix Matrix = View.ViewMatrices.GetViewToClip();
		FPlane LeftPlane(ForceInit), RightPlane(ForceInit), TopPlane(ForceInit), BottomPlane(ForceInit);
		Matrix.GetFrustumLeftPlane(LeftPlane);
		Matrix.GetFrustumRightPlane(RightPlane);
		Matrix.GetFrustumTopPlane(TopPlane);
		Matrix.GetFrustumBottomPlane(BottomPlane);

		OutPlanes[0] = FVector4f(FVector4(LeftPlane));
		OutPlanes[1] = FVector4f(FVector4(RightPlane));
		OutPlanes[2] = FVector4f(FVector4(TopPlane));
		OutPlanes[3] = FVector4f(FVector4(BottomPlane));
	};

	if constexpr (std::is_same_v<VSType, FVoxelDebugBoxVS>)
	{
		FVoxelDebugBoxParameters VSParams;
		VSParams.View = View.ViewUniformBuffer;
		VSParams.BoxDataBuffer = SRV;
		SetFrustumPlanes(VSParams.FrustumPlanes);

		SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), VSParams);

		FVoxelDebugBoxPS::FParameters PSParams;
		PSParams.Common = VSParams;
		SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PSParams);
	}
	else if constexpr (std::is_same_v<VSType, FVoxelDebugSphericalSectorVS>)
	{
		FVoxelDebugSphericalSectorParameters VSParams;
		VSParams.View = View.ViewUniformBuffer;
		VSParams.SectorDataBuffer = SRV;
		SetFrustumPlanes(VSParams.FrustumPlanes);

		SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), VSParams);

		FVoxelDebugSphericalSectorPS::FParameters PSParams;
		PSParams.Common = VSParams;
		SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PSParams);
	}
	else if constexpr (std::is_same_v<VSType, FVoxelDebugSphereVS>)
	{
		FVoxelDebugSphereParameters VSParams;
		VSParams.View = View.ViewUniformBuffer;
		VSParams.SphereDataBuffer = SRV;
		SetFrustumPlanes(VSParams.FrustumPlanes);

		SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), VSParams);

		FVoxelDebugSpherePS::FParameters PSParams;
		PSParams.Common = VSParams;
		SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PSParams);
	}
	else if constexpr (std::is_same_v<VSType, FVoxelDebugLineVS>)
	{
		FVoxelDebugLineParameters VSParams;
		VSParams.View = View.ViewUniformBuffer;
		VSParams.LineDataBuffer = SRV;
		SetFrustumPlanes(VSParams.FrustumPlanes);

		SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), VSParams);

		FVoxelDebugLinePS::FParameters PSParams;
		PSParams.Common = VSParams;
		SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PSParams);
	}
	else
	{
		FVoxelDebugPointParameters VSParams;
		VSParams.View = View.ViewUniformBuffer;
		VSParams.PointDataBuffer = SRV;

		SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), VSParams);

		FVoxelDebugPointPS::FParameters PSParams;
		PSParams.Common = VSParams;
		SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PSParams);
	}

	if constexpr (std::is_same_v<VSType, FVoxelDebugBoxVS>)
	{
		RHICmdList.DrawIndexedPrimitive(GVoxelDebugBoxIndexBuffer.IndexBufferRHI, 0, 0, VERTS_PER_BOX, 0, QUADS_PER_BOX * 2, NumInstances);
	}
	else if constexpr (std::is_same_v<VSType, FVoxelDebugSphericalSectorVS>)
	{
		RHICmdList.DrawIndexedPrimitive(GVoxelDebugSphericalSectorIndexBuffer.IndexBufferRHI, 0, 0, VERTS_PER_SECTOR, 0, QUADS_PER_SECTOR * 2, NumInstances);
	}
	else if constexpr (std::is_same_v<VSType, FVoxelDebugSphereVS>)
	{
		RHICmdList.DrawIndexedPrimitive(GVoxelDebugSphereIndexBuffer.IndexBufferRHI, 0, 0, VERTS_PER_SPHERE, 0, QUADS_PER_SPHERE * 2, NumInstances);
	}
	else
	{
		RHICmdList.DrawIndexedPrimitive(GTwoTrianglesIndexBuffer.IndexBufferRHI, 0, 0, 4, 0, 2, NumInstances);
	}
}

void FVoxelDebugDrawerManager::RenderEditorPrimitivesForWorld(
	FRHICommandList& RHICmdList,
	const FSceneView& View,
	bool bForeground,
	FRHIDepthStencilState* DepthStencilState)
{
	if (!View.bIsViewInfo)
	{
		return;
	}

	const UWorld* World_Unsafe = View.Family->Scene->GetWorld();
	const FViewInfo& ViewInfo = static_cast<const FViewInfo&>(View);

	for (const TSharedRef<FVoxelDebugDrawerWorldManager>& Manager : FVoxelDebugDrawerWorldManager::GetAll())
	{
		if (Manager->World_Unsafe != World_Unsafe)
		{
			continue;
		}

		auto RenderBufferSet = [&](const FVoxelDebugDrawerWorldManager::FBufferSet& Buffers)
		{
			if (bForeground)
			{
				DrawWithSRV<FVoxelDebugPointVS, FVoxelDebugPointPS>(RHICmdList, ViewInfo, Buffers.ForegroundPointBufferSRV, Buffers.ForegroundPointsToRender_RenderThread ? Buffers.ForegroundPointsToRender_RenderThread->Num() : 0, DepthStencilState);
				DrawWithSRV<FVoxelDebugLineVS, FVoxelDebugLinePS>(RHICmdList, ViewInfo, Buffers.ForegroundLineBufferSRV, Buffers.ForegroundLinesToRender_RenderThread ? Buffers.ForegroundLinesToRender_RenderThread->Num() : 0, DepthStencilState);
				DrawWithSRV<FVoxelDebugSphereVS, FVoxelDebugSpherePS>(RHICmdList, ViewInfo, Buffers.ForegroundSphereBufferSRV, Buffers.ForegroundSpheresToRender_RenderThread ? Buffers.ForegroundSpheresToRender_RenderThread->Num() : 0, DepthStencilState);
				DrawWithSRV<FVoxelDebugSphericalSectorVS, FVoxelDebugSphericalSectorPS>(RHICmdList, ViewInfo, Buffers.ForegroundSphericalSectorBufferSRV, Buffers.ForegroundSphericalSectorsToRender_RenderThread ? Buffers.ForegroundSphericalSectorsToRender_RenderThread->Num() : 0, DepthStencilState);
				DrawWithSRV<FVoxelDebugBoxVS, FVoxelDebugBoxPS>(RHICmdList, ViewInfo, Buffers.ForegroundBoxBufferSRV, Buffers.ForegroundBoxesToRender_RenderThread ? Buffers.ForegroundBoxesToRender_RenderThread->Num() : 0, DepthStencilState);
			}
			else
			{
				DrawWithSRV<FVoxelDebugPointVS, FVoxelDebugPointPS>(RHICmdList, ViewInfo, Buffers.PointBufferSRV, Buffers.PointsToRender_RenderThread ? Buffers.PointsToRender_RenderThread->Num() : 0, DepthStencilState);
				DrawWithSRV<FVoxelDebugLineVS, FVoxelDebugLinePS>(RHICmdList, ViewInfo, Buffers.LineBufferSRV, Buffers.LinesToRender_RenderThread ? Buffers.LinesToRender_RenderThread->Num() : 0, DepthStencilState);
				DrawWithSRV<FVoxelDebugSphereVS, FVoxelDebugSpherePS>(RHICmdList, ViewInfo, Buffers.SphereBufferSRV, Buffers.SpheresToRender_RenderThread ? Buffers.SpheresToRender_RenderThread->Num() : 0, DepthStencilState);
				DrawWithSRV<FVoxelDebugSphericalSectorVS, FVoxelDebugSphericalSectorPS>(RHICmdList, ViewInfo, Buffers.SphericalSectorBufferSRV, Buffers.SphericalSectorsToRender_RenderThread ? Buffers.SphericalSectorsToRender_RenderThread->Num() : 0, DepthStencilState);
				DrawWithSRV<FVoxelDebugBoxVS, FVoxelDebugBoxPS>(RHICmdList, ViewInfo, Buffers.BoxBufferSRV, Buffers.BoxesToRender_RenderThread ? Buffers.BoxesToRender_RenderThread->Num() : 0, DepthStencilState);
			}
		};

		RenderBufferSet(Manager->StaticBuffers);
		RenderBufferSet(Manager->DynamicBuffers);
	}
}

bool FVoxelDebugDrawerManager::ShouldCompositeEditorPrimitives_RenderThread(const FSceneView& View) const
{
	if (!View.bIsViewInfo)
	{
		return false;
	}

	const UWorld* ViewWorld = View.Family->Scene->GetWorld();

	for (const TSharedRef<FVoxelDebugDrawerWorldManager>& Manager : FVoxelDebugDrawerWorldManager::GetAll())
	{
		if (Manager->World_Unsafe != ViewWorld)
		{
			continue;
		}

		Manager->WaitForFlattenAndApply(View.Family->FrameNumber);

		auto HasDraws = [](const FVoxelDebugDrawerWorldManager::FBufferSet& Buffers)
		{
			return (Buffers.PointsToRender_RenderThread && Buffers.PointsToRender_RenderThread->Num() > 0) ||
				(Buffers.LinesToRender_RenderThread && Buffers.LinesToRender_RenderThread->Num() > 0) ||
				(Buffers.SpheresToRender_RenderThread && Buffers.SpheresToRender_RenderThread->Num() > 0) ||
				(Buffers.ForegroundPointsToRender_RenderThread && Buffers.ForegroundPointsToRender_RenderThread->Num() > 0) ||
				(Buffers.ForegroundLinesToRender_RenderThread && Buffers.ForegroundLinesToRender_RenderThread->Num() > 0) ||
				(Buffers.ForegroundSpheresToRender_RenderThread && Buffers.ForegroundSpheresToRender_RenderThread->Num() > 0) ||
				(Buffers.SphericalSectorsToRender_RenderThread && Buffers.SphericalSectorsToRender_RenderThread->Num() > 0) ||
				(Buffers.ForegroundSphericalSectorsToRender_RenderThread && Buffers.ForegroundSphericalSectorsToRender_RenderThread->Num() > 0) ||
				(Buffers.BoxesToRender_RenderThread && Buffers.BoxesToRender_RenderThread->Num() > 0) ||
				(Buffers.ForegroundBoxesToRender_RenderThread && Buffers.ForegroundBoxesToRender_RenderThread->Num() > 0);
		};

		if (HasDraws(Manager->StaticBuffers) || HasDraws(Manager->DynamicBuffers))
		{
			return true;
		}
	}

	return false;
}

void FVoxelDebugDrawerManager::PrepareEditorPrimitiveResources_RenderThread(
	FRDGBuilder& GraphBuilder,
	const FSceneView& View,
	FSceneViewExtensionBase::FEditorPrimitiveBufferBindings& Bindings)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInRenderingThread());

	PrepareBuffersForWorld(GraphBuilder, View, Bindings);
}

void FVoxelDebugDrawerManager::RenderEditorPrimitivesOpaque_RenderThread(
	FRHICommandList& RHICmdList,
	const FSceneView& View)
{
	VOXEL_FUNCTION_COUNTER();

	RenderEditorPrimitivesForWorld(RHICmdList, View, false, TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI());
}

void FVoxelDebugDrawerManager::RenderEditorPrimitivesForegroundOverwrite_RenderThread(
	FRHICommandList& RHICmdList,
	const FSceneView& View)
{
	VOXEL_FUNCTION_COUNTER();

	RenderEditorPrimitivesForWorld(RHICmdList, View, true, TStaticDepthStencilState<true, CF_Always>::GetRHI());
}

void FVoxelDebugDrawerManager::RenderEditorPrimitivesForegroundDepthTest_RenderThread(
	FRHICommandList& RHICmdList,
	const FSceneView& View)
{
	VOXEL_FUNCTION_COUNTER();

	RenderEditorPrimitivesForWorld(RHICmdList, View, true, TStaticDepthStencilState<true, CF_DepthNearOrEqual>::GetRHI());
}