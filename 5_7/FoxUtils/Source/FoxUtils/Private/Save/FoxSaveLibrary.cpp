// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Save/FoxSaveLibrary.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"

static FString GetFoxINIPath(const FString& FileName)
{
	return FPaths::ProjectSavedDir() / TEXT("Config") / (FileName + TEXT(".ini"));
}

static void EnsureConfigReady(const FString& Path)
{
	// Ensure directory exists
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectoryTree(*FPaths::GetPath(Path));

	// Ensure config file is registered in GConfig (required in UE5+)
	if (!GConfig->FindConfigFile(Path))
	{
		// Create an empty file so GConfig can track it
		if (!PlatformFile.FileExists(*Path))
		{
			FFileHelper::SaveStringToFile(TEXT(""), *Path);
		}
		GConfig->LoadFile(Path);
	}
}

void UFoxSaveLibrary::SaveStringToINI(const FString& Section, const FString& Key, const FString& Value, const FString& FileName)
{
	FString Path = GetFoxINIPath(FileName);
	EnsureConfigReady(Path);
	GConfig->SetString(*Section, *Key, *Value, Path);
	GConfig->Flush(false, Path);
}

FString UFoxSaveLibrary::LoadStringFromINI(const FString& Section, const FString& Key, const FString& Default, const FString& FileName)
{
	FString Path = GetFoxINIPath(FileName);
	EnsureConfigReady(Path);
	FString Value;
	if (GConfig->GetString(*Section, *Key, Value, Path))
	{
		return Value;
	}
	return Default;
}

void UFoxSaveLibrary::SaveIntToINI(const FString& Section, const FString& Key, int32 Value, const FString& FileName)
{
	SaveStringToINI(Section, Key, FString::FromInt(Value), FileName);
}

int32 UFoxSaveLibrary::LoadIntFromINI(const FString& Section, const FString& Key, int32 Default, const FString& FileName)
{
	FString Path = GetFoxINIPath(FileName);
	int32 Value;
	if (GConfig->GetInt(*Section, *Key, Value, Path))
	{
		return Value;
	}
	return Default;
}

void UFoxSaveLibrary::SaveFloatToINI(const FString& Section, const FString& Key, float Value, const FString& FileName)
{
	SaveStringToINI(Section, Key, FString::SanitizeFloat(Value), FileName);
}

float UFoxSaveLibrary::LoadFloatFromINI(const FString& Section, const FString& Key, float Default, const FString& FileName)
{
	FString Path = GetFoxINIPath(FileName);
	float Value;
	if (GConfig->GetFloat(*Section, *Key, Value, Path))
	{
		return Value;
	}
	return Default;
}
