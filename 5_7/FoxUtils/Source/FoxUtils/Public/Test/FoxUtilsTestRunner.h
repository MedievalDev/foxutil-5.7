// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoxUtilsTestRunner.generated.h"

/**
 * Test Runner Actor for FoxUtils world-dependent nodes.
 * Place in level or spawn via FoxUtils.CreateTestRunner, then press Play.
 * On BeginPlay, runs all Actor, Trace, Camera, Loop, Audio, InstancedMesh tests
 * and prints results to screen + output log.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "FoxUtils Test Runner"))
class FOXUTILS_API AFoxUtilsTestRunner : public AActor
{
	GENERATED_BODY()

public:
	AFoxUtilsTestRunner();

protected:
	virtual void BeginPlay() override;

private:
	int32 Passed;
	int32 Failed;
	int32 Total;

	void LogTest(const FString& Category, const FString& Name, bool bSuccess, const FString& Detail);
	void PrintSummary();

	void RunActorTests();
	void RunTraceTests();
	void RunCameraTests();
	void RunDebugTests();
};
