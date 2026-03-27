// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LatentActions.h"
#include "Engine/LatentActionManager.h"
#include "FoxLoopLibrary.generated.h"

UENUM(BlueprintType)
enum class EFoxLoopOutput : uint8
{
	LoopBody,
	Completed
};

UCLASS()
class FOXUTILS_API UFoxLoopLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * For loop with delay between each iteration.
	 * Fires LoopBody exec pin once per step, then Completed when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Loop", meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContext",
		DisplayName = "Fox For Loop with Delay", ExpandEnumAsExecs = "Outputs"))
	static void ForLoopWithDelay(UObject* WorldContext, int32 FirstIndex, int32 LastIndex, float DelayPerStep,
		EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo);

	/**
	 * For Each loop with delay between each element.
	 * Outputs the current element index on each iteration.
	 */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Loop", meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContext",
		DisplayName = "Fox For Each with Delay", ExpandEnumAsExecs = "Outputs"))
	static void ForEachWithDelay(UObject* WorldContext, int32 ArrayLength, float DelayPerStep,
		EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo);

	/**
	 * Batched for loop: processes BatchSize elements per frame with optional delay every batch.
	 */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Loop", meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContext",
		DisplayName = "Fox Batched Loop with Delay", ExpandEnumAsExecs = "Outputs"))
	static void BatchedLoopWithDelay(UObject* WorldContext, int32 FirstIndex, int32 LastIndex, int32 BatchSize, float DelayPerBatch,
		EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo);

	/** Reversed For Loop with Delay (LastIndex down to FirstIndex) */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Loop", meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContext",
		DisplayName = "Fox For Loop with Delay (Reversed)", ExpandEnumAsExecs = "Outputs"))
	static void ForLoopWithDelayReversed(UObject* WorldContext, int32 FirstIndex, int32 LastIndex, float DelayPerStep,
		EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo);

	/** Reversed For Each with Delay */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Loop", meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContext",
		DisplayName = "Fox For Each with Delay (Reversed)", ExpandEnumAsExecs = "Outputs"))
	static void ForEachWithDelayReversed(UObject* WorldContext, int32 ArrayLength, float DelayPerStep,
		EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo);

	/** Cancel a running delayed loop by its UUID */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Loop", meta = (WorldContext = "WorldContext", DisplayName = "Cancel Delayed Loop"))
	static void CancelDelayedLoop(UObject* WorldContext, int32 UUID);
};

// Internal latent action
class FFoxDelayedLoopAction : public FPendingLatentAction
{
public:
	int32 CurrentIndex;
	int32 EndIndex;
	int32 Step; // +1 or -1
	int32 BatchSize;
	float Delay;
	float TimeRemaining;
	int32 BatchCounter;
	bool bFirstTick;

	int32& IndexRef;
	EFoxLoopOutput& OutputRef;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

	FFoxDelayedLoopAction(int32 InStart, int32 InEnd, int32 InStep, int32 InBatch, float InDelay,
		int32& InIndexRef, EFoxLoopOutput& InOutputRef, const FLatentActionInfo& InInfo)
		: CurrentIndex(InStart), EndIndex(InEnd), Step(InStep), BatchSize(FMath::Max(1, InBatch))
		, Delay(InDelay), TimeRemaining(0.f), BatchCounter(0), bFirstTick(true)
		, IndexRef(InIndexRef), OutputRef(InOutputRef)
		, ExecutionFunction(InInfo.ExecutionFunction), OutputLink(InInfo.Linkage), CallbackTarget(InInfo.CallbackTarget)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		if (bFirstTick)
		{
			bFirstTick = false;
			// Fire first iteration immediately
			if (IsInRange())
			{
				IndexRef = CurrentIndex;
				OutputRef = EFoxLoopOutput::LoopBody;
				CurrentIndex += Step;
				BatchCounter++;
				Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
				if (BatchSize > 1 && BatchCounter < BatchSize && IsInRange())
				{
					return; // More in this batch
				}
				BatchCounter = 0;
				TimeRemaining = Delay;
				return;
			}
		}

		if (!IsInRange())
		{
			OutputRef = EFoxLoopOutput::Completed;
			Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
			Response.DoneIf(true);
			return;
		}

		TimeRemaining -= Response.ElapsedTime();
		if (TimeRemaining <= 0.f)
		{
			IndexRef = CurrentIndex;
			OutputRef = EFoxLoopOutput::LoopBody;
			CurrentIndex += Step;
			BatchCounter++;
			Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);

			if (BatchSize > 1 && BatchCounter < BatchSize && IsInRange())
			{
				return; // More in this batch, no delay
			}
			BatchCounter = 0;
			TimeRemaining = Delay;
		}
	}

	bool IsInRange() const
	{
		if (Step > 0) return CurrentIndex <= EndIndex;
		return CurrentIndex >= EndIndex;
	}

#if WITH_EDITOR
	virtual FString GetDescription() const override
	{
		return FString::Printf(TEXT("FoxDelayedLoop: %d→%d (at %d)"), CurrentIndex - Step, EndIndex, CurrentIndex);
	}
#endif
};
