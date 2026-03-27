// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoxDebugLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxDebugLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Enhanced print: message + color + duration + category prefix + optional log */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Debug", meta = (WorldContext = "WorldContext", DisplayName = "Fox Print", DevelopmentOnly, AdvancedDisplay = "2"))
	static void FoxPrint(UObject* WorldContext, const FString& Message, FLinearColor Color = FLinearColor::Green, float Duration = 5.f, const FString& Category = TEXT(""), bool bAlsoLog = true);

	/** Log only (no screen print) with verbosity */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Debug", meta = (DisplayName = "Fox Log", DevelopmentOnly))
	static void FoxLog(const FString& Message, const FString& Category = TEXT("FoxUtils"), bool bWarning = false);

	/** Auto-format and print a vector with label */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Debug", meta = (WorldContext = "WorldContext", DisplayName = "Print Vector", DevelopmentOnly))
	static void PrintVector(UObject* WorldContext, const FString& Label, FVector Value, FLinearColor Color = FLinearColor::Yellow, float Duration = 5.f);

	/** Start a performance timer with a label */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Debug", meta = (WorldContext = "WorldContext", DisplayName = "Perf Timer Start", DevelopmentOnly))
	static void PerfTimerStart(UObject* WorldContext, const FString& Label);

	/** Stop a performance timer and return elapsed milliseconds */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Debug", meta = (WorldContext = "WorldContext", DisplayName = "Perf Timer Stop", DevelopmentOnly))
	static float PerfTimerStop(UObject* WorldContext, const FString& Label, bool bPrintResult = true);

	/** Print only if condition is true — saves a Branch node */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Debug", meta = (WorldContext = "WorldContext", DisplayName = "Conditional Print", DevelopmentOnly))
	static void ConditionalPrint(UObject* WorldContext, bool bCondition, const FString& Message, FLinearColor Color = FLinearColor::White, float Duration = 5.f);

private:
	static TMap<FString, double> ActiveTimers;
};
