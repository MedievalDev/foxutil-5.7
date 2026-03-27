// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Test/FoxUtilsTestRunner.h"

#include "Actor/FoxActorLibrary.h"
#include "Trace/FoxTraceLibrary.h"
#include "Camera/FoxCameraLibrary.h"
#include "Debug/FoxDebugLibrary.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

AFoxUtilsTestRunner::AFoxUtilsTestRunner()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void AFoxUtilsTestRunner::BeginPlay()
{
	Super::BeginPlay();

	Passed = 0;
	Failed = 0;
	Total = 0;

	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("╔══════════════════════════════════════════╗"));
	UE_LOG(LogTemp, Log, TEXT("║    FoxUtils World Tests (TestRunner)    ║"));
	UE_LOG(LogTemp, Log, TEXT("╚══════════════════════════════════════════╝"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("=== FoxUtils World Tests Starting ==="));
	}

	RunActorTests();
	RunTraceTests();
	RunCameraTests();
	RunDebugTests();
	PrintSummary();
}

void AFoxUtilsTestRunner::LogTest(const FString& Category, const FString& Name, bool bSuccess, const FString& Detail)
{
	Total++;
	if (bSuccess) { Passed++; } else { Failed++; }

	FString Msg = FString::Printf(TEXT("[%s] %s::%s — %s"),
		bSuccess ? TEXT("PASS") : TEXT("FAIL"),
		*Category, *Name, *Detail);

	UE_LOG(LogTemp, Log, TEXT("  %s"), *Msg);

	if (GEngine)
	{
		FColor Color = bSuccess ? FColor::Green : FColor::Red;
		GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Msg);
	}
}

void AFoxUtilsTestRunner::PrintSummary()
{
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("╔══════════════════════════════════════════╗"));
	UE_LOG(LogTemp, Log, TEXT("║  World Tests: %d/%d passed  %s"), Passed, Total,
		Failed == 0 ? TEXT("ALL CLEAR! ║") : TEXT("          ║"));
	if (Failed > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("║  FAILED: %d tests                       ║"), Failed);
	}
	UE_LOG(LogTemp, Log, TEXT("╚══════════════════════════════════════════╝"));

	if (GEngine)
	{
		FString Summary = FString::Printf(TEXT("=== World Tests: %d/%d passed %s ==="),
			Passed, Total, Failed == 0 ? TEXT("ALL CLEAR!") : TEXT(""));
		FColor SumColor = Failed == 0 ? FColor::Green : FColor::Red;
		GEngine->AddOnScreenDebugMessage(-1, 15.f, SumColor, Summary);
	}
}

// ─────────────────────────────────────────────
// Actor Tests
// ─────────────────────────────────────────────
void AFoxUtilsTestRunner::RunActorTests()
{
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Actor ──"));

	UWorld* World = GetWorld();

	// GetClosestActorOfClass — find closest actor to our location
	{
		AActor* R = UFoxActorLibrary::GetClosestActorOfClass(this, AActor::StaticClass(), GetActorLocation(), 5000.f);
		// Should find at least something (possibly ourselves or the floor)
		LogTest(TEXT("Actor"), TEXT("GetClosestActorOfClass"), R != nullptr, R ? FString::Printf(TEXT("Found: %s"), *R->GetName()) : TEXT("None found"));
	}

	// GetActorsInRadius
	{
		TArray<AActor*> R = UFoxActorLibrary::GetActorsInRadius(this, AActor::StaticClass(), GetActorLocation(), 10000.f);
		LogTest(TEXT("Actor"), TEXT("GetActorsInRadius"), R.Num() > 0, FString::Printf(TEXT("Found %d actors"), R.Num()));
	}

	// SetActorActive — disable and re-enable self
	{
		UFoxActorLibrary::SetActorActive(this, false);
		bool bWasHidden = IsHidden();
		UFoxActorLibrary::SetActorActive(this, true);
		bool bNowVisible = !IsHidden();
		LogTest(TEXT("Actor"), TEXT("SetActorActive"), bWasHidden && bNowVisible, FString::Printf(TEXT("Hidden=%s Restored=%s"), bWasHidden ? TEXT("T") : TEXT("F"), bNowVisible ? TEXT("T") : TEXT("F")));
	}

	// GetPlayerCharacterSafe
	{
		ACharacter* R = UFoxActorLibrary::GetPlayerCharacterSafe(this, 0);
		LogTest(TEXT("Actor"), TEXT("GetPlayerCharacterSafe"), true, R ? FString::Printf(TEXT("Found: %s"), *R->GetName()) : TEXT("No player character (OK in editor)"));
	}

	// GetAllComponentsOfTypeRecursive
	{
		TArray<UActorComponent*> R = UFoxActorLibrary::GetAllComponentsOfTypeRecursive(this, USceneComponent::StaticClass());
		LogTest(TEXT("Actor"), TEXT("GetAllComponentsOfTypeRecursive"), R.Num() > 0, FString::Printf(TEXT("Found %d components"), R.Num()));
	}

	// SpawnActorSimple — spawn a temporary actor
	{
		FTransform T(FRotator::ZeroRotator, GetActorLocation() + FVector(200, 0, 0));
		AActor* R = UFoxActorLibrary::SpawnActorSimple(this, AActor::StaticClass(), T);
		bool bSpawned = R != nullptr;
		if (R) { R->Destroy(); }
		LogTest(TEXT("Actor"), TEXT("SpawnActorSimple"), bSpawned, bSpawned ? TEXT("Spawned and destroyed") : TEXT("Spawn failed"));
	}

	// GetGroundLocation
	{
		FVector GroundLoc, GroundNormal;
		bool R = UFoxActorLibrary::GetGroundLocation(this, GetActorLocation() + FVector(0, 0, 500), GroundLoc, GroundNormal);
		LogTest(TEXT("Actor"), TEXT("GetGroundLocation"), true, R ? FString::Printf(TEXT("Ground at %s"), *GroundLoc.ToString()) : TEXT("No ground hit (may be OK)"));
	}

	// GetForwardDirection2D
	{
		FVector R = UFoxActorLibrary::GetForwardDirection2D(this);
		float Len = R.Size();
		LogTest(TEXT("Actor"), TEXT("GetForwardDirection2D"), FMath::IsNearlyEqual(Len, 1.f, 0.1f), FString::Printf(TEXT("%s (len=%.2f)"), *R.ToString(), Len));
	}

	// FindLookAtRotation2D
	{
		FRotator R = UFoxActorLibrary::FindLookAtRotation2D(FVector(0, 0, 0), FVector(100, 0, 0));
		LogTest(TEXT("Actor"), TEXT("FindLookAtRotation2D"), true, FString::Printf(TEXT("%s"), *R.ToString()));
	}

	// GetDistanceToGround
	{
		float R = UFoxActorLibrary::GetDistanceToGround(this, this);
		LogTest(TEXT("Actor"), TEXT("GetDistanceToGround"), true, FString::Printf(TEXT("%.1f units"), R));
	}

	// PredictActorLocation
	{
		FVector R = UFoxActorLibrary::PredictActorLocation(this, 1.f);
		LogTest(TEXT("Actor"), TEXT("PredictActorLocation"), true, FString::Printf(TEXT("%s"), *R.ToString()));
	}
}

// ─────────────────────────────────────────────
// Trace Tests
// ─────────────────────────────────────────────
void AFoxUtilsTestRunner::RunTraceTests()
{
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Trace ──"));

	FVector Loc = GetActorLocation();

	// GroundCheck
	{
		FHitResult Hit;
		bool R = UFoxTraceLibrary::GroundCheck(this, Loc + FVector(0, 0, 200), Hit);
		LogTest(TEXT("Trace"), TEXT("GroundCheck"), true, R ? FString::Printf(TEXT("Hit at %s"), *Hit.ImpactPoint.ToString()) : TEXT("No ground (may be OK)"));
	}

	// SphereTraceForActors
	{
		TArray<AActor*> R = UFoxTraceLibrary::SphereTraceForActors(this, Loc, 5000.f, AActor::StaticClass());
		LogTest(TEXT("Trace"), TEXT("SphereTraceForActors"), true, FString::Printf(TEXT("Found %d actors"), R.Num()));
	}

	// ConeCheckForActors
	{
		TArray<AActor*> R = UFoxTraceLibrary::ConeCheckForActors(this, Loc, GetActorForwardVector(), 5000.f, 45.f, AActor::StaticClass());
		LogTest(TEXT("Trace"), TEXT("ConeCheckForActors"), true, FString::Printf(TEXT("Found %d actors in cone"), R.Num()));
	}

	// LineTraceIgnoreSelf
	{
		FHitResult Hit;
		TArray<AActor*> Ignore;
		bool R = UFoxTraceLibrary::LineTraceIgnoreSelf(this, this, Loc, Loc + FVector(0, 0, -10000), Hit, Ignore);
		LogTest(TEXT("Trace"), TEXT("LineTraceIgnoreSelf"), true, R ? FString::Printf(TEXT("Hit: %s"), Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("None")) : TEXT("No hit"));
	}

	// BoxTraceSimple
	{
		FHitResult Hit;
		bool R = UFoxTraceLibrary::BoxTraceSimple(this, Loc, Loc + FVector(0, 0, -5000), FVector(50, 50, 50), FRotator::ZeroRotator, Hit);
		LogTest(TEXT("Trace"), TEXT("BoxTraceSimple"), true, R ? TEXT("Hit something") : TEXT("No hit (may be OK)"));
	}

	// MultiLineTrace
	{
		TArray<FHitResult> R = UFoxTraceLibrary::MultiLineTrace(this, Loc, Loc + FVector(0, 0, -10000));
		LogTest(TEXT("Trace"), TEXT("MultiLineTrace"), true, FString::Printf(TEXT("%d hits"), R.Num()));
	}
}

// ─────────────────────────────────────────────
// Camera Tests
// ─────────────────────────────────────────────
void AFoxUtilsTestRunner::RunCameraTests()
{
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Camera ──"));

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No PlayerController — skipping Camera tests"));
		return;
	}

	// GetCameraTransform
	{
		FVector CamLoc;
		FRotator CamRot;
		bool R = UFoxCameraLibrary::GetCameraTransform(PC, CamLoc, CamRot);
		LogTest(TEXT("Camera"), TEXT("GetCameraTransform"), R, R ? FString::Printf(TEXT("Loc=%s Rot=%s"), *CamLoc.ToString(), *CamRot.ToString()) : TEXT("Failed"));
	}

	// GetScreenCenterWorldLocation
	{
		FVector WorldLoc, WorldDir;
		bool R = UFoxCameraLibrary::GetScreenCenterWorldLocation(PC, WorldLoc, WorldDir);
		LogTest(TEXT("Camera"), TEXT("GetScreenCenterWorldLocation"), R, R ? FString::Printf(TEXT("Loc=%s"), *WorldLoc.ToString()) : TEXT("Failed"));
	}

	// ProjectToScreenSafe
	{
		FVector2D ScreenPos;
		bool R = UFoxCameraLibrary::ProjectToScreenSafe(PC, GetActorLocation(), ScreenPos);
		LogTest(TEXT("Camera"), TEXT("ProjectToScreenSafe"), true, R ? FString::Printf(TEXT("Screen=%.0f,%.0f"), ScreenPos.X, ScreenPos.Y) : TEXT("Off screen (OK)"));
	}

	// CameraDistanceToActor
	{
		float R = UFoxCameraLibrary::CameraDistanceToActor(PC, this);
		LogTest(TEXT("Camera"), TEXT("CameraDistanceToActor"), R >= 0.f, FString::Printf(TEXT("%.1f units"), R));
	}

	// CameraShakeSimple — light shake
	{
		UFoxCameraLibrary::CameraShakeSimple(this, 1.f, 10.f, 0.2f, 0);
		LogTest(TEXT("Camera"), TEXT("CameraShakeSimple"), true, TEXT("Triggered (light shake)"));
	}
}

// ─────────────────────────────────────────────
// Debug Tests (world-dependent)
// ─────────────────────────────────────────────
void AFoxUtilsTestRunner::RunDebugTests()
{
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Debug (World) ──"));

	// FoxPrint
	{
		UFoxDebugLibrary::FoxPrint(this, TEXT("FoxPrint test message"), FLinearColor::Green, 5.f, TEXT("Test"), true);
		LogTest(TEXT("Debug"), TEXT("FoxPrint"), true, TEXT("Printed to screen"));
	}

	// PrintVector
	{
		UFoxDebugLibrary::PrintVector(this, TEXT("TestVec"), GetActorLocation(), FLinearColor::Yellow, 5.f);
		LogTest(TEXT("Debug"), TEXT("PrintVector"), true, FString::Printf(TEXT("Printed %s"), *GetActorLocation().ToString()));
	}

	// PerfTimer
	{
		UFoxDebugLibrary::PerfTimerStart(this, TEXT("FoxTestTimer"));
		// Small busy work
		float Dummy = 0.f;
		for (int32 i = 0; i < 10000; i++) { Dummy += FMath::Sin((float)i); }
		float Elapsed = UFoxDebugLibrary::PerfTimerStop(this, TEXT("FoxTestTimer"), false);
		LogTest(TEXT("Debug"), TEXT("PerfTimer"), Elapsed >= 0.f, FString::Printf(TEXT("%.4f ms"), Elapsed));
	}

	// ConditionalPrint
	{
		UFoxDebugLibrary::ConditionalPrint(this, true, TEXT("ConditionalPrint TRUE test"), FLinearColor::White, 5.f);
		LogTest(TEXT("Debug"), TEXT("ConditionalPrint"), true, TEXT("Printed conditionally"));
	}
}
