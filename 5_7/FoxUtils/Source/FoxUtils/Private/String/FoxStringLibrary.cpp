// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "String/FoxStringLibrary.h"

FString UFoxStringLibrary::TruncateString(const FString& Source, int32 MaxLength, const FString& Suffix)
{
	if (MaxLength <= 0) return FString();
	if (Source.Len() <= MaxLength) return Source;
	return Source.Left(MaxLength) + Suffix;
}

bool UFoxStringLibrary::ContainsIgnoreCase(const FString& Source, const FString& SubString)
{
	return Source.Contains(SubString, ESearchCase::IgnoreCase);
}

FString UFoxStringLibrary::ToTitleCase(const FString& Source)
{
	FString Result = Source.ToLower();
	bool bCapNext = true;
	for (int32 i = 0; i < Result.Len(); ++i)
	{
		if (bCapNext && FChar::IsAlpha(Result[i]))
		{
			Result[i] = FChar::ToUpper(Result[i]);
			bCapNext = false;
		}
		else if (FChar::IsWhitespace(Result[i]))
		{
			bCapNext = true;
		}
	}
	return Result;
}

bool UFoxStringLibrary::IsEmptyOrWhitespace(const FString& Source)
{
	if (Source.IsEmpty()) return true;
	for (int32 i = 0; i < Source.Len(); ++i)
	{
		if (!FChar::IsWhitespace(Source[i])) return false;
	}
	return true;
}

FString UFoxStringLibrary::FormatNumber(int64 Number, const FString& Separator)
{
	bool bNeg = Number < 0;
	if (bNeg) Number = -Number;
	FString NumStr = FString::Printf(TEXT("%lld"), Number);
	FString Result;
	int32 Count = 0;
	for (int32 i = NumStr.Len() - 1; i >= 0; --i)
	{
		if (Count > 0 && Count % 3 == 0)
		{
			Result = Separator + Result;
		}
		Result = FString::Chr(NumStr[i]) + Result;
		++Count;
	}
	return bNeg ? TEXT("-") + Result : Result;
}

FString UFoxStringLibrary::FormatTimeSeconds(float TotalSeconds, bool bUseLetters, bool bShowMilliseconds)
{
	if (TotalSeconds < 0.f) TotalSeconds = 0.f;
	int32 Hours = FMath::FloorToInt(TotalSeconds / 3600.f);
	int32 Minutes = FMath::FloorToInt(FMath::Fmod(TotalSeconds, 3600.f) / 60.f);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(TotalSeconds, 60.f));
	int32 Ms = FMath::FloorToInt(FMath::Fmod(TotalSeconds * 1000.f, 1000.f));

	if (bUseLetters)
	{
		FString R;
		if (Hours > 0) R += FString::Printf(TEXT("%dh "), Hours);
		R += FString::Printf(TEXT("%02dm %02ds"), Minutes, Seconds);
		if (bShowMilliseconds) R += FString::Printf(TEXT(" %03dms"), Ms);
		return R;
	}
	FString R;
	if (Hours > 0) R = FString::Printf(TEXT("%d:%02d:%02d"), Hours, Minutes, Seconds);
	else R = FString::Printf(TEXT("%d:%02d"), Minutes, Seconds);
	if (bShowMilliseconds) R += FString::Printf(TEXT(".%03d"), Ms);
	return R;
}

FString UFoxStringLibrary::RemoveWhitespace(const FString& Source)
{
	FString Result;
	Result.Reserve(Source.Len());
	for (int32 i = 0; i < Source.Len(); ++i)
	{
		if (!FChar::IsWhitespace(Source[i]))
		{
			Result.AppendChar(Source[i]);
		}
	}
	return Result;
}

FString UFoxStringLibrary::PadString(const FString& Source, int32 MinLength, const FString& PadChar, bool bPadLeft)
{
	if (Source.Len() >= MinLength || PadChar.IsEmpty()) return Source;
	FString Padding;
	int32 PadCount = MinLength - Source.Len();
	for (int32 i = 0; i < PadCount; ++i)
	{
		Padding += PadChar[0];
	}
	return bPadLeft ? Padding + Source : Source + Padding;
}

bool UFoxStringLibrary::SplitFirst(const FString& Source, const FString& Delimiter, FString& Left, FString& Right)
{
	return Source.Split(Delimiter, &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
}

FString UFoxStringLibrary::JoinStringArray(const TArray<FString>& Array, const FString& Separator)
{
	return FString::Join(Array, *Separator);
}
