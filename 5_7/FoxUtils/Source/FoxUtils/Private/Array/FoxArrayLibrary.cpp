// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Array/FoxArrayLibrary.h"

TArray<int32> UFoxArrayLibrary::ShuffleIntArray(const TArray<int32>& Array)
{
	TArray<int32> R = Array;
	for (int32 i = R.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		R.Swap(i, j);
	}
	return R;
}

TArray<FString> UFoxArrayLibrary::ShuffleStringArray(const TArray<FString>& Array)
{
	TArray<FString> R = Array;
	for (int32 i = R.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		R.Swap(i, j);
	}
	return R;
}

TArray<int32> UFoxArrayLibrary::RemoveDuplicateInts(const TArray<int32>& Array)
{
	TSet<int32> Seen;
	TArray<int32> R;
	R.Reserve(Array.Num());
	for (int32 V : Array)
	{
		bool bAlready = false;
		Seen.Add(V, &bAlready);
		if (!bAlready) R.Add(V);
	}
	return R;
}

TArray<FString> UFoxArrayLibrary::RemoveDuplicateStrings(const TArray<FString>& Array)
{
	TSet<FString> Seen;
	TArray<FString> R;
	R.Reserve(Array.Num());
	for (const FString& V : Array)
	{
		bool bAlready = false;
		Seen.Add(V, &bAlready);
		if (!bAlready) R.Add(V);
	}
	return R;
}

int32 UFoxArrayLibrary::RandomIntElement(UPARAM(ref) TArray<int32>& Array, bool bRemove, bool& bSuccess)
{
	if (Array.Num() == 0) { bSuccess = false; return 0; }
	bSuccess = true;
	int32 Idx = FMath::RandRange(0, Array.Num() - 1);
	int32 Val = Array[Idx];
	if (bRemove) Array.RemoveAt(Idx);
	return Val;
}

TArray<int32> UFoxArrayLibrary::RandomIntElements(const TArray<int32>& Array, int32 Count)
{
	TArray<int32> Pool = Array;
	TArray<int32> R;
	Count = FMath::Min(Count, Pool.Num());
	for (int32 i = 0; i < Count; ++i)
	{
		int32 Idx = FMath::RandRange(0, Pool.Num() - 1);
		R.Add(Pool[Idx]);
		Pool.RemoveAt(Idx);
	}
	return R;
}

TArray<int32> UFoxArrayLibrary::SortIntArray(const TArray<int32>& Array, bool bDescending)
{
	TArray<int32> R = Array;
	R.Sort([bDescending](int32 A, int32 B) { return bDescending ? A > B : A < B; });
	return R;
}

TArray<float> UFoxArrayLibrary::SortFloatArray(const TArray<float>& Array, bool bDescending)
{
	TArray<float> R = Array;
	R.Sort([bDescending](float A, float B) { return bDescending ? A > B : A < B; });
	return R;
}

TArray<FString> UFoxArrayLibrary::SortStringArray(const TArray<FString>& Array, bool bDescending)
{
	TArray<FString> R = Array;
	R.Sort([bDescending](const FString& A, const FString& B) { return bDescending ? A > B : A < B; });
	return R;
}

int32 UFoxArrayLibrary::WeightedRandomIndex(const TArray<float>& Weights)
{
	if (Weights.Num() == 0) return -1;
	float Total = 0.f;
	for (float W : Weights) Total += FMath::Max(0.f, W);
	if (Total <= 0.f) return FMath::RandRange(0, Weights.Num() - 1);
	float Roll = FMath::FRandRange(0.f, Total);
	float Accum = 0.f;
	for (int32 i = 0; i < Weights.Num(); ++i)
	{
		Accum += FMath::Max(0.f, Weights[i]);
		if (Roll <= Accum) return i;
	}
	return Weights.Num() - 1;
}
