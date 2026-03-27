// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoxActorLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxActorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Get the closest actor of a given class to a location */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Actor", meta = (WorldContext = "WorldContext", DeterminesOutputType = "ActorClass", DisplayName = "Get Closest Actor of Class"))
	static AActor* GetClosestActorOfClass(UObject* WorldContext, TSubclassOf<AActor> ActorClass, FVector Location, float MaxRadius = 0.f);

	/** Get all actors of class within radius, sorted by distance */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Actor", meta = (WorldContext = "WorldContext", DisplayName = "Get Actors in Radius"))
	static TArray<AActor*> GetActorsInRadius(UObject* WorldContext, TSubclassOf<AActor> ActorClass, FVector Location, float Radius);

	/** Is this actor currently visible on the player's screen? */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (DisplayName = "Is Actor On Screen"))
	static bool IsActorOnScreen(AActor* Actor, APlayerController* PC);

	/** Set actor hidden, collision, and tick in one call */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Actor", meta = (DisplayName = "Set Actor Active"))
	static void SetActorActive(AActor* Actor, bool bActive);

	/** Get player character with null safety (no crash if invalid) */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (WorldContext = "WorldContext", DisplayName = "Get Player Character (Safe)"))
	static ACharacter* GetPlayerCharacterSafe(UObject* WorldContext, int32 PlayerIndex = 0);

	/** Get all components of type on actor, including child actors */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Actor", meta = (DisplayName = "Get All Components of Type (Recursive)", DeterminesOutputType = "ComponentClass"))
	static TArray<UActorComponent*> GetAllComponentsOfTypeRecursive(AActor* Actor, TSubclassOf<UActorComponent> ComponentClass);

	/** Simplified deferred spawn: class + transform, returns spawned actor */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Actor", meta = (WorldContext = "WorldContext", DeterminesOutputType = "ActorClass", DisplayName = "Spawn Actor Simple"))
	static AActor* SpawnActorSimple(UObject* WorldContext, TSubclassOf<AActor> ActorClass, FTransform SpawnTransform);

	/** Line trace straight down from location, returns ground position */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (WorldContext = "WorldContext", DisplayName = "Get Ground Location"))
	static bool GetGroundLocation(UObject* WorldContext, FVector StartLocation, FVector& GroundLocation, FVector& GroundNormal, float TraceDistance = 10000.f);

	/** Is Actor A facing Actor B within AngleTolerance degrees? */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (DisplayName = "Is Facing Actor"))
	static bool IsFacingActor(AActor* Source, AActor* Target, float AngleToleranceDeg = 45.f);

	/** Get smooth look-at rotation toward target (use with SetActorRotation per tick) */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (DisplayName = "Look At Rotation (Smooth)"))
	static FRotator LookAtSmooth(AActor* Source, FVector TargetLocation, float InterpSpeed, float DeltaTime);

	/** Get forward direction on XY plane (Z zeroed, normalized) */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (DisplayName = "Get Forward Direction 2D"))
	static FVector GetForwardDirection2D(AActor* Actor);

	/** Predict where an actor will be in N seconds based on current velocity */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (DisplayName = "Predict Actor Location"))
	static FVector PredictActorLocation(AActor* Actor, float TimeAhead);

	/** How far above the ground is this actor? */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (WorldContext = "WorldContext", DisplayName = "Get Distance to Ground"))
	static float GetDistanceToGround(UObject* WorldContext, AActor* Actor, float MaxTrace = 10000.f);

	/** Look-at rotation ignoring Z axis (for turrets, top-down games) */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Actor", meta = (DisplayName = "Find Look At Rotation 2D"))
	static FRotator FindLookAtRotation2D(FVector Source, FVector Target);
};
