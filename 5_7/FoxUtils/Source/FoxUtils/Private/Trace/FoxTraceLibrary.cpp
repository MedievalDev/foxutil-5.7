// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Trace/FoxTraceLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

TArray<AActor*> UFoxTraceLibrary::SphereTraceForActors(UObject* WorldContext, FVector Origin, float Radius, TSubclassOf<AActor> FilterClass)
{
	TArray<AActor*> Result;
	if (!WorldContext || !FilterClass) return Result;

	TArray<AActor*> All;
	UGameplayStatics::GetAllActorsOfClass(WorldContext, FilterClass, All);

	float RadSq = Radius * Radius;
	for (AActor* A : All)
	{
		if (A && FVector::DistSquared(A->GetActorLocation(), Origin) <= RadSq)
		{
			Result.Add(A);
		}
	}

	Result.Sort([&Origin](const AActor& A, const AActor& B)
	{
		return FVector::DistSquared(A.GetActorLocation(), Origin) < FVector::DistSquared(B.GetActorLocation(), Origin);
	});

	return Result;
}

TArray<AActor*> UFoxTraceLibrary::ConeCheckForActors(UObject* WorldContext, FVector Origin, FVector Direction, float Range, float HalfAngleDeg, TSubclassOf<AActor> FilterClass)
{
	TArray<AActor*> Result;
	if (!WorldContext || !FilterClass) return Result;

	TArray<AActor*> All;
	UGameplayStatics::GetAllActorsOfClass(WorldContext, FilterClass, All);

	FVector DirNorm = Direction.GetSafeNormal();
	float CosAngle = FMath::Cos(FMath::DegreesToRadians(HalfAngleDeg));
	float RangeSq = Range * Range;

	for (AActor* A : All)
	{
		if (!A) continue;
		FVector ToActor = A->GetActorLocation() - Origin;
		if (ToActor.SizeSquared() > RangeSq) continue;

		FVector ToActorNorm = ToActor.GetSafeNormal();
		float Dot = FVector::DotProduct(DirNorm, ToActorNorm);
		if (Dot >= CosAngle)
		{
			Result.Add(A);
		}
	}

	Result.Sort([&Origin](const AActor& A, const AActor& B)
	{
		return FVector::DistSquared(A.GetActorLocation(), Origin) < FVector::DistSquared(B.GetActorLocation(), Origin);
	});

	return Result;
}

bool UFoxTraceLibrary::GroundCheck(UObject* WorldContext, FVector Location, FHitResult& HitResult, float TraceDepth, bool bComplex)
{
	if (!WorldContext) return false;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return false;

	FCollisionQueryParams Params;
	Params.bTraceComplex = bComplex;
	Params.bReturnPhysicalMaterial = true;

	FVector End = Location - FVector(0.f, 0.f, TraceDepth);
	return World->LineTraceSingleByChannel(HitResult, Location, End, ECC_Visibility, Params);
}

bool UFoxTraceLibrary::WallCheck(UObject* WorldContext, AActor* Actor, FHitResult& HitResult, float TraceLength, float HeightOffset)
{
	if (!WorldContext || !Actor) return false;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return false;

	FVector Start = Actor->GetActorLocation() + FVector(0.f, 0.f, HeightOffset);
	FVector End = Start + Actor->GetActorForwardVector() * TraceLength;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Actor);

	return World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
}

bool UFoxTraceLibrary::LineTraceIgnoreSelf(UObject* WorldContext, AActor* SourceActor, FVector Start, FVector End, FHitResult& HitResult, const TArray<AActor*>& AdditionalIgnore)
{
	if (!WorldContext) return false;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return false;

	FCollisionQueryParams Params;
	if (SourceActor) Params.AddIgnoredActor(SourceActor);
	for (AActor* A : AdditionalIgnore)
	{
		if (A) Params.AddIgnoredActor(A);
	}

	return World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
}

bool UFoxTraceLibrary::VisibilityCheck(UObject* WorldContext, AActor* From, AActor* To, const TArray<AActor*>& IgnoreActors)
{
	if (!WorldContext || !From || !To) return false;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return false;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(From);
	Params.AddIgnoredActor(To);
	for (AActor* A : IgnoreActors)
	{
		if (A) Params.AddIgnoredActor(A);
	}

	FHitResult Hit;
	bool bBlocked = World->LineTraceSingleByChannel(Hit, From->GetActorLocation(), To->GetActorLocation(), ECC_Visibility, Params);
	return !bBlocked;
}

bool UFoxTraceLibrary::BoxTraceSimple(UObject* WorldContext, FVector Start, FVector End, FVector HalfExtent, FRotator Orientation, FHitResult& HitResult)
{
	if (!WorldContext) return false;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return false;

	FCollisionQueryParams Params;
	FCollisionShape Box = FCollisionShape::MakeBox(HalfExtent);

	return World->SweepSingleByChannel(HitResult, Start, End, Orientation.Quaternion(), ECC_Visibility, Box, Params);
}

TArray<FHitResult> UFoxTraceLibrary::MultiLineTrace(UObject* WorldContext, FVector Start, FVector End, bool bTraceComplex)
{
	TArray<FHitResult> Results;
	if (!WorldContext) return Results;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return Results;

	FCollisionQueryParams Params;
	Params.bTraceComplex = bTraceComplex;

	World->LineTraceMultiByChannel(Results, Start, End, ECC_Visibility, Params);
	return Results;
}
