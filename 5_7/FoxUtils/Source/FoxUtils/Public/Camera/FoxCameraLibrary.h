// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoxCameraLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxCameraLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Simple camera shake with amplitude, frequency, and duration */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Camera", meta = (WorldContext = "WorldContext", DisplayName = "Camera Shake Simple"))
	static void CameraShakeSimple(UObject* WorldContext, float Amplitude = 5.f, float Frequency = 20.f, float Duration = 0.5f, int32 PlayerIndex = 0);

	/** Get the world location and direction of the center of the screen */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Camera", meta = (DisplayName = "Get Screen Center World Location"))
	static bool GetScreenCenterWorldLocation(APlayerController* PC, FVector& WorldLocation, FVector& WorldDirection);

	/** Project world location to screen with safety check (returns false if behind camera) */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Camera", meta = (DisplayName = "Project to Screen (Safe)"))
	static bool ProjectToScreenSafe(APlayerController* PC, FVector WorldLocation, FVector2D& ScreenPosition);

	/** Get distance from player camera to an actor */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Camera", meta = (DisplayName = "Camera Distance to Actor"))
	static float CameraDistanceToActor(APlayerController* PC, AActor* Actor);

	/** Smooth camera look-at: returns rotation to interpolate toward target point */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Camera", meta = (DisplayName = "Camera Look At Rotation"))
	static FRotator CameraLookAtRotation(APlayerController* PC, FVector TargetLocation, float InterpSpeed, float DeltaTime);

	/** Get the player camera's current world transform */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Camera", meta = (DisplayName = "Get Camera Transform"))
	static bool GetCameraTransform(APlayerController* PC, FVector& Location, FRotator& Rotation);
};
