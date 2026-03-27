// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Camera/FoxCameraLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"

void UFoxCameraLibrary::CameraShakeSimple(UObject* WorldContext, float Amplitude, float Frequency, float Duration, int32 PlayerIndex)
{
	if (!WorldContext) return;
	APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContext, PlayerIndex);
	if (!PC || !PC->PlayerCameraManager) return;

	// Load MatineeCameraShake at runtime to avoid compile-time GameplayCameras plugin dependency
	static TSubclassOf<UCameraShakeBase> ShakeClass;
	if (!ShakeClass)
	{
		ShakeClass = LoadClass<UCameraShakeBase>(nullptr, TEXT("/Script/GameplayCameras.MatineeCameraShake"));
	}

	if (ShakeClass)
	{
		PC->PlayerCameraManager->StartCameraShake(ShakeClass, Amplitude / 10.f, ECameraShakePlaySpace::CameraLocal, FRotator::ZeroRotator);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FoxUtils: CameraShakeSimple requires the GameplayCameras plugin to be enabled."));
	}
}

bool UFoxCameraLibrary::GetScreenCenterWorldLocation(APlayerController* PC, FVector& WorldLocation, FVector& WorldDirection)
{
	if (!PC) return false;
	int32 SX, SY;
	PC->GetViewportSize(SX, SY);
	return PC->DeprojectScreenPositionToWorld((float)SX * 0.5f, (float)SY * 0.5f, WorldLocation, WorldDirection);
}

bool UFoxCameraLibrary::ProjectToScreenSafe(APlayerController* PC, FVector WorldLocation, FVector2D& ScreenPosition)
{
	if (!PC) return false;

	// Check if point is in front of camera
	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);
	FVector ToTarget = WorldLocation - CamLoc;
	float Dot = FVector::DotProduct(CamRot.Vector(), ToTarget);
	if (Dot <= 0.f) return false; // Behind camera

	return PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition);
}

float UFoxCameraLibrary::CameraDistanceToActor(APlayerController* PC, AActor* Actor)
{
	if (!PC || !Actor) return -1.f;
	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);
	return FVector::Dist(CamLoc, Actor->GetActorLocation());
}

FRotator UFoxCameraLibrary::CameraLookAtRotation(APlayerController* PC, FVector TargetLocation, float InterpSpeed, float DeltaTime)
{
	if (!PC) return FRotator::ZeroRotator;
	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);
	FRotator TargetRot = (TargetLocation - CamLoc).Rotation();
	return FMath::RInterpTo(CamRot, TargetRot, DeltaTime, InterpSpeed);
}

bool UFoxCameraLibrary::GetCameraTransform(APlayerController* PC, FVector& Location, FRotator& Rotation)
{
	if (!PC) return false;
	PC->GetPlayerViewPoint(Location, Rotation);
	return true;
}
