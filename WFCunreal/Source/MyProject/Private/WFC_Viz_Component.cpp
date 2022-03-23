#include "WFC_Viz_Component.h"

void FWFC_DebugRenderSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
	const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	FDebugRenderSceneProxy::GetDynamicMeshElements(Views, ViewFamily, VisibilityMap, Collector);


	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];
			FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

			// Draw Circles
			for (const auto& C : Circles)
			{
				DrawCircle(PDI, C.Centre, C.X, C.Y, C.Color, C.Radius, C.NumSegments, SDPG_World, C.Thickness, 0, C.Thickness > 0);
			}

			// Draw Arcs
			for (const auto& C : Arcs)
			{
				::DrawArc(PDI, 
					C.Centre, 
					C.X, C.Y, 
					C.MinAngle, C.MaxAngle,
					C.Radius, C.NumSegments,
					C.Color, SDPG_Foreground);	
			}
		}
	}
}
FPrimitiveViewRelevance FWFC_DebugRenderSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	// More useful defaults than FDebugRenderSceneProxy
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bDynamicRelevance = true;
	Result.bShadowRelevance = false;
	Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
	return Result;
}

UWFC_Viz_Component::UWFC_Viz_Component(const FObjectInitializer& ObjectInitializer)
	: UPrimitiveComponent(ObjectInitializer)
{
	// set up some constants
	PrimaryComponentTick.bCanEverTick = false;
	SetCastShadow(false);
#if WITH_EDITORONLY_DATA
	// Note: this makes this component invisible on level instances, not sure why
	SetIsVisualizationComponent(true);
#endif
	SetHiddenInGame(true);
	bVisibleInReflectionCaptures = false;
	bVisibleInRayTracing = false;
	bVisibleInRealTimeSkyCaptures = false;
	AlwaysLoadOnClient = false;
	bIsEditorOnly = true;
	
}

FPrimitiveSceneProxy* UWFC_Viz_Component::CreateSceneProxy()
{
	auto Ret = new FWFC_DebugRenderSceneProxy(this);

	const FTransform& XForm = GetComponentTransform();
	for (auto& L : Lines)
	{
		Ret->Lines.Add(FDebugRenderSceneProxy::FDebugLine(XForm.TransformPosition(L.Start),
		                                                  XForm.TransformPosition(L.End), L.Colour));
	}
	for (auto& A : Arrows)
	{
		Ret->ArrowLines.Add(FDebugRenderSceneProxy::FArrowLine(XForm.TransformPosition(A.Start),
														XForm.TransformPosition(A.End), A.Colour));
	}
	for (auto& C : Circles)
	{
		FQuat WorldRot = XForm.TransformRotation(C.Rotation.Quaternion());
		Ret->Circles.Add(FWFC_DebugRenderSceneProxy::FDebugCircle(
			XForm.TransformPosition(C.Location),
			WorldRot.GetForwardVector(), WorldRot.GetRightVector(),
			XForm.GetMaximumAxisScale() * C.Radius,
			C.NumSegments, C.Colour
			));
	}
	for (auto& Arc : Arcs)
	{
		FQuat WorldRot = XForm.TransformRotation(Arc.Rotation.Quaternion());
		Ret->Arcs.Add(FWFC_DebugRenderSceneProxy::FDebugArc(
			XForm.TransformPosition(Arc.Location),
			WorldRot.GetForwardVector(), WorldRot.GetRightVector(),
			Arc.MinAngle, Arc.MaxAngle,
			XForm.GetMaximumAxisScale() * Arc.Radius,
			Arc.NumSegments, Arc.Colour
			));
	}
	for (auto& S : Spheres)
	{
		Ret->Spheres.Add(FWFC_DebugRenderSceneProxy::FSphere(
			XForm.GetMaximumAxisScale() * S.Radius,
			XForm.TransformPosition(S.Location),
			S.Colour
			));
	}
	for (auto& Box : Boxes)
	{
		FVector HalfSize = Box.Size * 0.5f;
		FBox DBox(-HalfSize, HalfSize);
		// Apply local rotation first then parent transform
		FTransform CombinedXForm = FTransform(Box.Rotation, Box.Location) * XForm;
		Ret->Boxes.Add(FWFC_DebugRenderSceneProxy::FDebugBox(
			DBox, Box.Colour, CombinedXForm));
	}

	return Ret;
	
}

FBoxSphereBounds UWFC_Viz_Component::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds B = Super::CalcBounds(LocalToWorld);

	// Now we need to merge in all components
	for (auto& L : Lines)
	{
		// Re-centre the origin of the line to make box extents 
		FVector Extents = L.Start.GetAbs().ComponentMax(L.End.GetAbs());
		B = B + FBoxSphereBounds(FVector::ZeroVector, Extents, Extents.GetMax());
	}
	for (auto& A : Arrows)
	{
		// Re-centre the origin of the line to make box extents 
		FVector Extents = A.Start.GetAbs().ComponentMax(A.End.GetAbs());
		B = B + FBoxSphereBounds(FVector::ZeroVector, Extents, Extents.GetMax());
	}
	for (auto& C : Circles)
	{
		B = B + FBoxSphereBounds(C.Location, FVector(C.Radius), C.Radius);
	}
	for (auto& Arc : Arcs)
	{
		// Just use the entire circle for simplicity
		B = B + FBoxSphereBounds(Arc.Location, FVector(Arc.Radius), Arc.Radius);
	}
	for (auto& S : Spheres)
	{
		B = B + FBoxSphereBounds(S.Location, FVector(S.Radius), S.Radius);
	}
	for (auto& Box : Boxes)
	{
		FVector HalfSize = Box.Size * 0.5f;
		FBox DBox(-HalfSize, HalfSize);
		// Apply local rotation only, world is done later
		FTransform BoxXForm = FTransform(Box.Rotation, Box.Location);
		DBox = DBox.TransformBy(BoxXForm);
		B = B + FBoxSphereBounds(DBox);
	}
	return B.TransformBy(LocalToWorld);
}