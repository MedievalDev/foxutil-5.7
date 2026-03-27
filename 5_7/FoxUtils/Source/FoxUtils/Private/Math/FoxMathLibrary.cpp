// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Math/FoxMathLibrary.h"

float UFoxMathLibrary::RemapRange(float Value, float InMin, float InMax, float OutMin, float OutMax, bool bClamp)
{
	if (FMath::IsNearlyEqual(InMax, InMin)) return OutMin;
	float T = (Value - InMin) / (InMax - InMin);
	if (bClamp) T = FMath::Clamp(T, 0.f, 1.f);
	return OutMin + T * (OutMax - OutMin);
}

float UFoxMathLibrary::RoundToDecimals(float Value, int32 Decimals)
{
	float Mult = FMath::Pow(10.f, (float)FMath::Max(0, Decimals));
	return FMath::RoundToFloat(Value * Mult) / Mult;
}

float UFoxMathLibrary::NormalizeAngle(float Angle, bool bSigned)
{
	Angle = FMath::Fmod(Angle, 360.f);
	if (Angle < 0.f) Angle += 360.f;
	if (bSigned && Angle > 180.f) Angle -= 360.f;
	return Angle;
}

float UFoxMathLibrary::Distance2D(FVector A, FVector B)
{
	return FVector2D(A.X - B.X, A.Y - B.Y).Size();
}

bool UFoxMathLibrary::IsNearlyEqualFloat(float A, float B, float Tolerance)
{
	return FMath::IsNearlyEqual(A, B, Tolerance);
}

int32 UFoxMathLibrary::SignFloat(float Value)
{
	if (Value > 0.f) return 1;
	if (Value < 0.f) return -1;
	return 0;
}

float UFoxMathLibrary::WrapFloat(float Value, float Min, float Max)
{
	if (FMath::IsNearlyEqual(Max, Min)) return Min;
	float Range = Max - Min;
	float Result = FMath::Fmod(Value - Min, Range);
	if (Result < 0.f) Result += Range;
	return Result + Min;
}

float UFoxMathLibrary::Percentage(float A, float B)
{
	if (FMath::IsNearlyZero(B)) return 0.f;
	return (A / B) * 100.f;
}
