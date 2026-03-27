// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Actor/FoxActorLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

AActor* UFoxActorLibrary::GetClosestActorOfClass(UObject* WorldContext, TSubclassOf<AActor> ActorClass, FVector Location, float MaxRadius)
{
	if (!WorldContext || !ActorClass) return nullptr;
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(WorldContext, ActorClass, Actors);
	AActor* Closest = nullptr;
	float ClosestDist = MaxRadius > 0.f ? MaxRadius * MaxRadius : MAX_FLT;
	for (AActor* A : Actors)
	{
		if (!A) continue;
		float D = FVector::DistSquared(A->GetActorLocation(), Location);
		if (D < ClosestDist) { ClosestDist = D; Closest = A; }
	}
	return Closest;
}

TArray<AActor*> UFoxActorLibrary::GetActorsInRadius(UObject* WorldContext, TSubclassOf<AActor> ActorClass, FVector Location, float Radius)
{
	TArray<AActor*> All, Result;
	if (!WorldContext || !ActorClass) return Result;
	UGameplayStatics::GetAllActorsOfClass(WorldContext, ActorClass, All);
	float RadSq = Radius * Radius;
	for (AActor* A : All)
	{
		if (A && FVector::DistSquared(A->GetActorLocation(), Location) <= RadSq)
			Result.Add(A);
	}
	Result.Sort([&Location](const AActor& A, const AActor& B)
	{
		return FVector::DistSquared(A.GetActorLocation(), Location) < FVector::DistSquared(B.GetActorLocation(), Location);
	});
	return Result;
}

bool UFoxActorLibrary::IsActorOnScreen(AActor* Actor, APlayerController* PC)
{
	if (!Actor || !PC) return false;
	FVector2D ScreenPos;
	bool bProjected = PC->ProjectWorldLocationToScreen(Actor->GetActorLocation(), ScreenPos);
	if (!bProjected) return false;
	int32 SX, SY;
	PC->GetViewportSize(SX, SY);
	return ScreenPos.X >= 0.f && ScreenPos.X <= (float)SX && ScreenPos.Y >= 0.f && ScreenPos.Y <= (float)SY;
}

void UFoxActorLibrary::SetActorActive(AActor* Actor, bool bActive)
{
	if (!Actor) return;
	Actor->SetActorHiddenInGame(!bActive);
	Actor->SetActorEnableCollision(bActive);
	Actor->SetActorTickEnabled(bActive);
}

ACharacter* UFoxActorLibrary::GetPlayerCharacterSafe(UObject* WorldContext, int32 PlayerIndex)
{
	if (!WorldContext) return nullptr;
	return Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(WorldContext, PlayerIndex));
}

TArray<UActorComponent*> UFoxActorLibrary::GetAllComponentsOfTypeRecursive(AActor* Actor, TSubclassOf<UActorComponent> ComponentClass)
{
	TArray<UActorComponent*> Result;
	if (!Actor || !ComponentClass) return Result;
	TArray<UActorComponent*> Comps;
	Actor->GetComponents(ComponentClass, Comps);
	Result.Append(Comps);
	TArray<AActor*> Children;
	Actor->GetAllChildActors(Children, true);
	for (AActor* Child : Children)
	{
		if (!Child) continue;
		TArray<UActorComponent*> ChildComps;
		Child->GetComponents(ComponentClass, ChildComps);
		Result.Append(ChildComps);
	}
	return Result;
}

AActor* UFoxActorLibrary::SpawnActorSimple(UObject* WorldContext, TSubclassOf<AActor> ActorClass, FTransform SpawnTransform)
{
	if (!WorldContext || !ActorClass) return nullptr;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	return World->SpawnActor<AActor>(ActorClass, SpawnTransform, Params);
}

bool UFoxActorLibrary::GetGroundLocation(UObject* WorldContext, FVector StartLocation, FVector& GroundLocation, FVector& GroundNormal, float TraceDistance)
{
	if (!WorldContext) return false;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return false;
	FHitResult Hit;
	FVector End = StartLocation - FVector(0.f, 0.f, TraceDistance);
	bool bHit = World->LineTraceSingleByChannel(Hit, StartLocation, End, ECC_Visibility);
	if (bHit) { GroundLocation = Hit.ImpactPoint; GroundNormal = Hit.ImpactNormal; }
	return bHit;
}

bool UFoxActorLibrary::IsFacingActor(AActor* Source, AActor* Target, float AngleToleranceDeg)
{
	if (!Source || !Target) return false;
	FVector Dir = (Target->GetActorLocation() - Source->GetActorLocation()).GetSafeNormal();
	FVector Fwd = Source->GetActorForwardVector();
	float Dot = FVector::DotProduct(Fwd, Dir);
	float AngleRad = FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f));
	return FMath::RadiansToDegrees(AngleRad) <= AngleToleranceDeg;
}

FRotator UFoxActorLibrary::LookAtSmooth(AActor* Source, FVector TargetLocation, float InterpSpeed, float DeltaTime)
{
	if (!Source) return FRotator::ZeroRotator;
	FRotator Current = Source->GetActorRotation();
	FRotator Target = (TargetLocation - Source->GetActorLocation()).Rotation();
	return FMath::RInterpTo(Current, Target, DeltaTime, InterpSpeed);
}

FVector UFoxActorLibrary::GetForwardDirection2D(AActor* Actor)
{
	if (!Actor) return FVector::ForwardVector;
	FVector Fwd = Actor->GetActorForwardVector();
	Fwd.Z = 0.f;
	return Fwd.GetSafeNormal();
}

FVector UFoxActorLibrary::PredictActorLocation(AActor* Actor, float TimeAhead)
{
	if (!Actor) return FVector::ZeroVector;
	FVector Vel = Actor->GetVelocity();
	return Actor->GetActorLocation() + Vel * TimeAhead;
}

float UFoxActorLibrary::GetDistanceToGround(UObject* WorldContext, AActor* Actor, float MaxTrace)
{
	if (!Actor || !WorldContext) return -1.f;
	FVector Start = Actor->GetActorLocation();
	FVector GroundLoc, GroundNorm;
	if (GetGroundLocation(WorldContext, Start, GroundLoc, GroundNorm, MaxTrace))
	{
		return FVector::Dist(Start, GroundLoc);
	}
	return -1.f;
}

FRotator UFoxActorLibrary::FindLookAtRotation2D(FVector Source, FVector Target)
{
	FVector Dir = Target - Source;
	Dir.Z = 0.f;
	if (Dir.IsNearlyZero()) return FRotator::ZeroRotator;
	return Dir.Rotation();
}
