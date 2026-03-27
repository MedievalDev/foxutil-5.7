// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "FoxTraceLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxTraceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Sphere trace returning all actors of class, sorted by distance */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Trace", meta = (WorldContext = "WorldContext", DisplayName = "Sphere Trace for Actors"))
	static TArray<AActor*> SphereTraceForActors(UObject* WorldContext, FVector Origin, float Radius, TSubclassOf<AActor> FilterClass);

	/** Cone-shaped visibility check: get all actors within angle and range */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Trace", meta = (WorldContext = "WorldContext", DisplayName = "Cone Check for Actors"))
	static TArray<AActor*> ConeCheckForActors(UObject* WorldContext, FVector Origin, FVector Direction, float Range, float HalfAngleDeg, TSubclassOf<AActor> FilterClass);

	/** Line trace straight down to find ground position, normal and physical material */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Trace", meta = (WorldContext = "WorldContext", DisplayName = "Ground Check"))
	static bool GroundCheck(UObject* WorldContext, FVector Location, FHitResult& HitResult, float TraceDepth = 10000.f, bool bComplex = false);

	/** Horizontal trace in actor's forward direction for wall detection */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Trace", meta = (WorldContext = "WorldContext", DisplayName = "Wall Check"))
	static bool WallCheck(UObject* WorldContext, AActor* Actor, FHitResult& HitResult, float TraceLength = 200.f, float HeightOffset = 0.f);

	/** Line trace with automatic ignore for self and optional additional actors */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Trace", meta = (WorldContext = "WorldContext", DisplayName = "Line Trace (Ignore Self)"))
	static bool LineTraceIgnoreSelf(UObject* WorldContext, AActor* SourceActor, FVector Start, FVector End, FHitResult& HitResult, const TArray<AActor*>& AdditionalIgnore);

	/** Check line of sight between two actors */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Trace", meta = (WorldContext = "WorldContext", DisplayName = "Visibility Check"))
	static bool VisibilityCheck(UObject* WorldContext, AActor* From, AActor* To, const TArray<AActor*>& IgnoreActors);

	/** Box trace along a direction */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Trace", meta = (WorldContext = "WorldContext", DisplayName = "Box Trace Simple"))
	static bool BoxTraceSimple(UObject* WorldContext, FVector Start, FVector End, FVector HalfExtent, FRotator Orientation, FHitResult& HitResult);

	/** Multi line trace returning all hits (not just first) */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Trace", meta = (WorldContext = "WorldContext", DisplayName = "Multi Line Trace"))
	static TArray<FHitResult> MultiLineTrace(UObject* WorldContext, FVector Start, FVector End, bool bTraceComplex = false);
};
