//Based on: https://github.com/sinbad/StevesUEHelpers
//Provides an actor component which can visualize shapes in the editor.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "DebugRenderSceneProxy.h"

#include "WFC_Viz_Component.generated.h"


USTRUCT(BlueprintType)
struct MYPROJECT_API FWFC_Viz_Line
{
	GENERATED_BODY()

	/// Start location relative to component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Start;
	/// End location relative to component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector End;
	/// The colour of the line render 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor Colour;

	FWFC_Viz_Line(const FVector& InStart, const FVector& InEnd,
	                     const FColor& InColour)
		: Start(InStart),
		  End(InEnd),
		  Colour(InColour)
	{
	}

	FWFC_Viz_Line():
		Start(FVector::ZeroVector),
		End(FVector(100, 0, 0)),
		Colour(FColor::White)
	{
	}
};

USTRUCT(BlueprintType)
struct MYPROJECT_API FWFC_Viz_Circle
{
	GENERATED_BODY()

	/// Location relative to component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
	/// Rotation relative to component; circles will be rendered in the X/Y plane
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;
	/// Circle radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
	/// The number of line segments to render the circle with
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumSegments;
	/// The colour of the line render 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor Colour;

	FWFC_Viz_Circle(const FVector& InLocation, const FRotator& InRotation, float InRadius, int InNumSegments,
	                       const FColor& InColour)
		: Location(InLocation),
		  Rotation(InRotation),
		  Radius(InRadius),
		  NumSegments(InNumSegments),
		  Colour(InColour)
	{
	}

	FWFC_Viz_Circle():
		Location(FVector::ZeroVector),
		Rotation(FRotator::ZeroRotator),
		Radius(50), NumSegments(12),
		Colour(FColor::White)
	{
	}
};

USTRUCT(BlueprintType)
struct MYPROJECT_API FWFC_Viz_Arc
{
	GENERATED_BODY()

	/// Location relative to component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
	/// Rotation relative to component; arcs will be rendered in the X/Y plane
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;
	/// Minimum angle to render arc from 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinAngle;
	/// Maximum angle to render arc to 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAngle;
	/// Circle radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
	/// The number of line segments to render the circle with
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumSegments;
	/// The colour of the line render 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor Colour;

	FWFC_Viz_Arc(const FVector& InLocation, const FRotator& InRotation, float InMinAngle, float InMaxAngle,
	                    float InRadius, int InNumSegments,
	                    const FColor& InColour)
		: Location(InLocation),
		  Rotation(InRotation),
		  MinAngle(InMinAngle),
		  MaxAngle(InMaxAngle),
		  Radius(InRadius),
		  NumSegments(InNumSegments),
		  Colour(InColour)
	{
	}

	FWFC_Viz_Arc():
		Location(FVector::ZeroVector),
		Rotation(FRotator::ZeroRotator),
		MinAngle(0),
		MaxAngle(180),
		Radius(50), NumSegments(12),
		Colour(FColor::White)
	{
	}
};

USTRUCT(BlueprintType)
struct MYPROJECT_API FWFC_Viz_Sphere
{
	GENERATED_BODY()

	/// Location relative to component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
	/// Sphere radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
	/// The colour of the line render 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor Colour;

	FWFC_Viz_Sphere(const FVector& InLocation, float InRadius, const FColor& InColour) :
		Location(InLocation),
		Radius(InRadius),
		Colour(InColour)
	{
	}

	FWFC_Viz_Sphere():
		Location(FVector::ZeroVector),
		Radius(50),
		Colour(FColor::White)
	{
	}
};

USTRUCT(BlueprintType)
struct MYPROJECT_API FWFC_Viz_Box
{
	GENERATED_BODY()

	/// Location relative to component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
	/// Size of box in each axis
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Size;
	/// Rotation relative to component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;
	/// The colour of the line render 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor Colour;

	FWFC_Viz_Box(const FVector& InLocation, const FVector& InSize, const FRotator& InRot,
	                    const FColor& InColour) :
		Location(InLocation),
		Size(InSize),
		Rotation(InRot),
		Colour(InColour)
	{
	}

	FWFC_Viz_Box():
		Location(FVector::ZeroVector),
		Size(FVector(50, 50, 50)),
		Rotation(FRotator::ZeroRotator),
		Colour(FColor::White)
	{
	}
};


/**
 * An extension to FDebugRenderSceneProxy to support other shapes, e.g. circles and arcs
 */
class FWFC_DebugRenderSceneProxy : public FDebugRenderSceneProxy
{
public:
	FWFC_DebugRenderSceneProxy(const UPrimitiveComponent* InComponent)
		: FDebugRenderSceneProxy(InComponent)
	{
	}

	MYPROJECT_API virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
													  const FSceneViewFamily& ViewFamily,
													  uint32 VisibilityMap,
													  FMeshElementCollector& Collector) const override;

	MYPROJECT_API virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	struct FDebugCircle
	{
		FDebugCircle(const FVector& InCentre, const FVector& InX, const FVector& InY,
					 float InRadius, int InNumSegments,
		             const FColor& InColor, float InThickness = 0) :
			Centre(InCentre),
			X(InX),
			Y(InY),
			Radius(InRadius),
			NumSegments(InNumSegments),
			Color(InColor),
			Thickness(InThickness)
		{
		}

		FVector Centre;
		FVector X;
		FVector Y;
		float Radius;
		int NumSegments;
		FColor Color;
		float Thickness;
	};

	/// An arc which is a section of a circle
	struct FDebugArc
	{
		FDebugArc(const FVector& InCentre, const FVector& InX, const FVector& InY,
		          float InMinAngle, float InMaxAngle,
		          float InRadius, int InNumSegments, const FColor& InColor) :
			Centre(InCentre),
			X(InX),
			Y(InY),
			MinAngle(InMinAngle),
			MaxAngle(InMaxAngle),
			Radius(InRadius),
			NumSegments(InNumSegments),
			Color(InColor)
		{
		}

		FVector Centre;
		FVector X;
		FVector Y;
		float MinAngle;
		float MaxAngle;
		float Radius;
		int NumSegments;
		FColor Color;
	};

	TArray<FDebugCircle> Circles;
	TArray<FDebugArc> Arcs;
};


/**
 *
 * A component that solely exists to provide arbitrary editor visualisation when not selected.
 * FComponentVisualizer can only display visualisation when selected. 
 * To display vis on an unselected object, you need a UPrimitiveComponent, and sometimes you don't want/need one of those
 * in your actor. Instead, add UWFC_Viz_Component at construction of your actor, or registration of another component,
 * but only in a WITH_EDITOR block. Then, get nice visualisation of your actor/component without making more invasive changes
 * to your code.
 *
 * If you want to, you can add this to your Blueprints too. This component automatically marks itself as "visualisation
 * only" so shouldn't have a runtime impact.
 */
UCLASS(Blueprintable, ClassGroup="Utility", HideCategories=(Collision,Physics,Object,LOD,Lighting,TextureStreaming),
	   meta=(DisplayName="WFC Visualisation Helper", BlueprintSpawnableComponent))
class MYPROJECT_API UWFC_Viz_Component : public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWFC_Viz_Line> Lines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWFC_Viz_Line> Arrows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWFC_Viz_Circle> Circles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWFC_Viz_Arc> Arcs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWFC_Viz_Sphere> Spheres;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWFC_Viz_Box> Boxes;

	UWFC_Viz_Component(const FObjectInitializer& ObjectInitializer);

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	/// Needs to update on transform since proxy is detached
	virtual bool ShouldRecreateProxyOnUpdateTransform() const override { return true; }
};