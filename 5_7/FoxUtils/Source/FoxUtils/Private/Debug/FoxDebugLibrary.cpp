// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Debug/FoxDebugLibrary.h"
#include "Engine/Engine.h"

TMap<FString, double> UFoxDebugLibrary::ActiveTimers;

void UFoxDebugLibrary::FoxPrint(UObject* WorldContext, const FString& Message, FLinearColor Color, float Duration, const FString& Category, bool bAlsoLog)
{
	FString Msg = Category.IsEmpty() ? Message : FString::Printf(TEXT("[%s] %s"), *Category, *Message);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, Color.ToFColor(true), Msg);
	if (bAlsoLog) UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
}

void UFoxDebugLibrary::FoxLog(const FString& Message, const FString& Category, bool bWarning)
{
	FString Msg = FString::Printf(TEXT("[%s] %s"), *Category, *Message);
	if (bWarning) { UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg); }
	else { UE_LOG(LogTemp, Log, TEXT("%s"), *Msg); }
}

void UFoxDebugLibrary::PrintVector(UObject* WorldContext, const FString& Label, FVector Value, FLinearColor Color, float Duration)
{
	FString Msg = FString::Printf(TEXT("%s: X=%.2f Y=%.2f Z=%.2f"), *Label, Value.X, Value.Y, Value.Z);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, Color.ToFColor(true), Msg);
}

void UFoxDebugLibrary::PerfTimerStart(UObject* WorldContext, const FString& Label)
{
	ActiveTimers.Add(Label, FPlatformTime::Seconds());
}

float UFoxDebugLibrary::PerfTimerStop(UObject* WorldContext, const FString& Label, bool bPrintResult)
{
	double* Start = ActiveTimers.Find(Label);
	if (!Start) return -1.f;
	float Ms = (float)((FPlatformTime::Seconds() - *Start) * 1000.0);
	ActiveTimers.Remove(Label);
	if (bPrintResult)
	{
		FString Msg = FString::Printf(TEXT("[PerfTimer] %s: %.3f ms"), *Label, Ms);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
		UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
	}
	return Ms;
}

void UFoxDebugLibrary::ConditionalPrint(UObject* WorldContext, bool bCondition, const FString& Message, FLinearColor Color, float Duration)
{
	if (bCondition && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color.ToFColor(true), Message);
	}
}
