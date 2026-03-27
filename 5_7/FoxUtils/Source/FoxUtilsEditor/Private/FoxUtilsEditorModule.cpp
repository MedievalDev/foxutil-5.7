// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "FoxUtilsEditorModule.h"
#include "Modules/ModuleManager.h"
#include "HAL/IConsoleManager.h"

// Blueprint graph includes
#include "K2Node_CallFunction.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphNode_Comment.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ConfigCacheIni.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif
#include "ObjectTools.h"
#include "UObject/Package.h"
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 2
#include "UObject/SavePackage.h"
#endif
#include "Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

// FoxUtils classes
#include "Actor/FoxActorLibrary.h"
#include "Array/FoxArrayLibrary.h"
#include "Audio/FoxAudioLibrary.h"
#include "Camera/FoxCameraLibrary.h"
#include "Debug/FoxDebugLibrary.h"
#include "InstancedMesh/FoxInstancedMeshLibrary.h"
#include "Inventory/FoxInventoryLibrary.h"
#include "Loop/FoxLoopLibrary.h"
#include "Math/FoxMathLibrary.h"
#include "Platform/FoxPlatformLibrary.h"
#include "Save/FoxSaveLibrary.h"
#include "String/FoxStringLibrary.h"
#include "Trace/FoxTraceLibrary.h"
#include "Test/FoxUtilsTestRunner.h"

// For debug commands
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "RHI.h"
#include "GenericPlatform/GenericPlatformMisc.h"

IMPLEMENT_MODULE(FFoxUtilsEditorModule, FoxUtilsEditor)

// ─────────────────────────────────────────────
// Catalog: category name, class, color, function names
// ─────────────────────────────────────────────
struct FFoxNodeEntry
{
	FName FunctionName;
};

struct FFoxCategoryEntry
{
	FString CategoryName;
	UClass* OwnerClass;
	FLinearColor Color;
	TArray<FFoxNodeEntry> Functions;
};

static TArray<FFoxCategoryEntry> BuildCatalog()
{
	TArray<FFoxCategoryEntry> C;

	auto Add = [&](const FString& Cat, UClass* Cls, FLinearColor Col, std::initializer_list<const char*> Funcs)
	{
		FFoxCategoryEntry E;
		E.CategoryName = Cat;
		E.OwnerClass = Cls;
		E.Color = Col;
		for (auto* F : Funcs) { E.Functions.Add({ FName(F) }); }
		C.Add(MoveTemp(E));
	};

	Add(TEXT("Actor"), UFoxActorLibrary::StaticClass(), FLinearColor(0.2f, 0.6f, 0.2f, 1.f), {
		"GetClosestActorOfClass", "GetActorsInRadius", "IsActorOnScreen", "SetActorActive",
		"GetPlayerCharacterSafe", "GetAllComponentsOfTypeRecursive", "SpawnActorSimple",
		"GetGroundLocation", "IsFacingActor", "LookAtSmooth", "GetForwardDirection2D",
		"PredictActorLocation", "GetDistanceToGround", "FindLookAtRotation2D"
	});

	Add(TEXT("Array"), UFoxArrayLibrary::StaticClass(), FLinearColor(0.6f, 0.3f, 0.1f, 1.f), {
		"ShuffleIntArray", "ShuffleStringArray", "RemoveDuplicateInts", "RemoveDuplicateStrings",
		"RandomIntElement", "RandomIntElements", "SortIntArray", "SortFloatArray",
		"SortStringArray", "WeightedRandomIndex"
	});

	Add(TEXT("Audio"), UFoxAudioLibrary::StaticClass(), FLinearColor(0.7f, 0.2f, 0.5f, 1.f), {
		"PlaySoundExtended", "PlayRandomSound", "FadeSoundVolume",
		"StopAllSoundsOnActor", "IsAnySoundPlaying"
	});

	Add(TEXT("Camera"), UFoxCameraLibrary::StaticClass(), FLinearColor(0.2f, 0.4f, 0.8f, 1.f), {
		"CameraShakeSimple", "GetScreenCenterWorldLocation", "ProjectToScreenSafe",
		"CameraDistanceToActor", "CameraLookAtRotation", "GetCameraTransform"
	});

	Add(TEXT("Debug"), UFoxDebugLibrary::StaticClass(), FLinearColor(0.9f, 0.7f, 0.1f, 1.f), {
		"FoxPrint", "FoxLog", "PrintVector", "PerfTimerStart", "PerfTimerStop", "ConditionalPrint"
	});

	Add(TEXT("Instanced Mesh"), UFoxInstancedMeshLibrary::StaticClass(), FLinearColor(0.4f, 0.7f, 0.4f, 1.f), {
		"HISMRefreshCustomData", "FISMRefreshCustomData", "HISMGetInstancesInRadius",
		"HISMGetNearestInstance", "HISMBatchAddInstances", "HISMBatchUpdateCustomData"
	});

	Add(TEXT("Inventory"), UFoxInventoryLibrary::StaticClass(), FLinearColor(0.8f, 0.5f, 0.1f, 1.f), {
		"CalculateStackSplit", "CanStackItems", "CalculateTotalWeight",
		"FitsInContainer", "FindBestFitSlot"
	});

	Add(TEXT("Loop"), UFoxLoopLibrary::StaticClass(), FLinearColor(0.1f, 0.5f, 0.7f, 1.f), {
		"ForLoopWithDelay", "ForEachWithDelay", "BatchedLoopWithDelay",
		"ForLoopWithDelayReversed", "ForEachWithDelayReversed", "CancelDelayedLoop"
	});

	Add(TEXT("Math"), UFoxMathLibrary::StaticClass(), FLinearColor(0.5f, 0.2f, 0.7f, 1.f), {
		"RemapRange", "RoundToDecimals", "NormalizeAngle", "Distance2D",
		"IsNearlyEqualFloat", "SignFloat", "WrapFloat", "Percentage"
	});

	Add(TEXT("Platform"), UFoxPlatformLibrary::StaticClass(), FLinearColor(0.6f, 0.6f, 0.6f, 1.f), {
		"IsPackagedBuild", "CopyToClipboard", "OpenURLInBrowser", "GetEngineVersionString",
		"GetPlatformName", "GetGPUName", "GetTotalRAM_MB", "GetTotalVRAM_MB",
		"GetCPUBrand", "GetDesktopResolution"
	});

	Add(TEXT("Save"), UFoxSaveLibrary::StaticClass(), FLinearColor(0.3f, 0.6f, 0.8f, 1.f), {
		"SaveStringToINI", "LoadStringFromINI", "SaveIntToINI",
		"LoadIntFromINI", "SaveFloatToINI", "LoadFloatFromINI"
	});

	Add(TEXT("String"), UFoxStringLibrary::StaticClass(), FLinearColor(0.8f, 0.3f, 0.3f, 1.f), {
		"TruncateString", "ContainsIgnoreCase", "ToTitleCase", "IsEmptyOrWhitespace",
		"FormatNumber", "FormatTimeSeconds", "RemoveWhitespace", "PadString",
		"SplitFirst", "JoinStringArray"
	});

	Add(TEXT("Trace"), UFoxTraceLibrary::StaticClass(), FLinearColor(0.9f, 0.4f, 0.1f, 1.f), {
		"SphereTraceForActors", "ConeCheckForActors", "GroundCheck", "WallCheck",
		"LineTraceIgnoreSelf", "VisibilityCheck", "BoxTraceSimple", "MultiLineTrace"
	});

	return C;
}

// ─────────────────────────────────────────────
// Module startup / shutdown
// ─────────────────────────────────────────────
void FFoxUtilsEditorModule::StartupModule()
{
	// FoxUtils.CreateShowcase
	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("FoxUtils.CreateShowcase"),
		TEXT("Creates BP_FoxUtils_Showcase with all FoxUtils nodes organized by category"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FFoxUtilsEditorModule::HandleCreateShowcase),
		ECVF_Default
	));

	// FoxDebug commands
	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("FoxDebug.ListNodes"),
		TEXT("Lists all FoxUtils nodes grouped by category"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FFoxUtilsEditorModule::HandleListNodes),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("FoxDebug.SystemInfo"),
		TEXT("Prints GPU, CPU, RAM, VRAM and platform info"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FFoxUtilsEditorModule::HandleSystemInfo),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("FoxDebug.CountActors"),
		TEXT("Counts all actors in current world by class. Usage: FoxDebug.CountActors [optional class filter]"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FFoxUtilsEditorModule::HandleCountActors),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("FoxDebug.TraceDebug"),
		TEXT("Fires a debug trace from the viewport camera and prints hit info"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FFoxUtilsEditorModule::HandleTraceDebug),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("FoxUtils.RunTests"),
		TEXT("Runs automated tests for all non-world FoxUtils nodes (String, Array, Math, Platform, Save, Inventory)"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FFoxUtilsEditorModule::HandleRunTests),
		ECVF_Default
	));

	ConsoleCommands.Add(IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("FoxUtils.CreateTestRunner"),
		TEXT("Spawns a FoxUtilsTestRunner actor in the current level. Press Play to run world-dependent tests."),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FFoxUtilsEditorModule::HandleCreateTestRunner),
		ECVF_Default
	));

	UE_LOG(LogTemp, Log, TEXT("FoxUtilsEditor: Loaded. Commands: FoxUtils.CreateShowcase, FoxUtils.RunTests, FoxUtils.CreateTestRunner, FoxDebug.*"));
}

void FFoxUtilsEditorModule::ShutdownModule()
{
	for (IConsoleObject* Cmd : ConsoleCommands)
	{
		IConsoleManager::Get().UnregisterConsoleObject(Cmd);
	}
	ConsoleCommands.Empty();
}

// ─────────────────────────────────────────────
// FoxUtils.CreateShowcase
// ─────────────────────────────────────────────
void FFoxUtilsEditorModule::HandleCreateShowcase(const TArray<FString>& Args)
{
	const FString PackagePath = TEXT("/Game/FoxUtils/");
	const FString AssetName = TEXT("BP_FoxUtils_Showcase");
	const FString FullPath = PackagePath + AssetName;

	// ── Load pre-formatted node text from plugin Resources ──
	const FString ResourcePath = FPaths::Combine(
		FPaths::ProjectPluginsDir(), TEXT("FoxUtils"), TEXT("Resources"), TEXT("ShowcaseNodes.txt"));

	FString NodeText;
	if (!FFileHelper::LoadFileToString(NodeText, *ResourcePath))
	{
		UE_LOG(LogTemp, Error, TEXT("FoxUtils: Could not load ShowcaseNodes.txt from %s"), *ResourcePath);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("FoxUtils: Loaded showcase layout (%d chars) from %s"),
		NodeText.Len(), *ResourcePath);

	// ── Delete existing asset if present ──
	{
		UObject* Existing = StaticFindObject(UBlueprint::StaticClass(), nullptr, *FullPath);
		if (!Existing)
		{
			Existing = LoadObject<UBlueprint>(nullptr, *FullPath);
		}
		if (Existing)
		{
			UE_LOG(LogTemp, Log, TEXT("FoxUtils: Removing existing BP_FoxUtils_Showcase..."));
			TArray<UObject*> ToDelete;
			ToDelete.Add(Existing);
			ObjectTools::ForceDeleteObjects(ToDelete, false);
		}
	}

	// ── Create package ──
#if ENGINE_MAJOR_VERSION >= 5
	UPackage* Package = CreatePackage(*(PackagePath + AssetName));
#else
	UPackage* Package = CreatePackage(nullptr, *(PackagePath + AssetName));
#endif
	if (!Package)
	{
		UE_LOG(LogTemp, Error, TEXT("FoxUtils: Could not create package at %s"), *FullPath);
		return;
	}
	Package->FullyLoad();

	// ── Create Blueprint ──
	UBlueprint* BP = FKismetEditorUtilities::CreateBlueprint(
		AActor::StaticClass(),
		Package,
		*AssetName,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass()
	);

	if (!BP)
	{
		UE_LOG(LogTemp, Error, TEXT("FoxUtils: Could not create Blueprint!"));
		return;
	}

	// ── Find Event Graph ──
	UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(BP);
	if (!EventGraph)
	{
		UE_LOG(LogTemp, Error, TEXT("FoxUtils: Could not find Event Graph!"));
		return;
	}

	// ── Remove default event nodes (BeginPlay, ActorBeginOverlap, Tick, etc.) ──
	{
		TArray<UEdGraphNode*> NodesToRemove;
		for (UEdGraphNode* Node : EventGraph->Nodes)
		{
			NodesToRemove.Add(Node);
		}
		for (UEdGraphNode* Node : NodesToRemove)
		{
			EventGraph->RemoveNode(Node);
		}
		UE_LOG(LogTemp, Log, TEXT("FoxUtils: Removed %d default nodes from Event Graph"), NodesToRemove.Num());
	}

	// ── Import pre-formatted nodes from text ──
	TSet<UEdGraphNode*> ImportedNodes;
	FEdGraphUtilities::ImportNodesFromText(EventGraph, NodeText, ImportedNodes);

	if (ImportedNodes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("FoxUtils: ImportNodesFromText produced 0 nodes! Check ShowcaseNodes.txt"));
		return;
	}

	// Reconstruct each node to properly initialize pins
	for (UEdGraphNode* Node : ImportedNodes)
	{
		Node->ReconstructNode();
	}

	UE_LOG(LogTemp, Log, TEXT("FoxUtils: Imported %d nodes into Event Graph"), ImportedNodes.Num());

	// ── Compile and save ──
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
	FKismetEditorUtilities::CompileBlueprint(BP);

	FAssetRegistryModule::AssetCreated(BP);
	Package->MarkPackageDirty();

	FString FilePath = FPackageName::LongPackageNameToFilename(FullPath, FPackageName::GetAssetPackageExtension());
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 2
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	FSavePackageResultStruct Result = UPackage::Save(Package, BP, *FilePath, SaveArgs);
	bool bSaved = Result == ESavePackageResult::Success;
#else
	bool bSaved = UPackage::SavePackage(Package, BP, RF_Public | RF_Standalone, *FilePath);
#endif
	if (!bSaved)
	{
		UE_LOG(LogTemp, Warning, TEXT("FoxUtils: SavePackage failed for %s"), *FilePath);
	}

	UE_LOG(LogTemp, Log, TEXT("=== FoxUtils Showcase Created! ==="));
	UE_LOG(LogTemp, Log, TEXT("  Asset: %s"), *FullPath);
	UE_LOG(LogTemp, Log, TEXT("  Nodes: %d imported"), ImportedNodes.Num());
	UE_LOG(LogTemp, Log, TEXT("  Layout: Pre-formatted from ShowcaseNodes.txt"));
	UE_LOG(LogTemp, Log, TEXT("  Open via Content Browser > /Game/FoxUtils/BP_FoxUtils_Showcase"));
}

// ─────────────────────────────────────────────
// FoxDebug.ListNodes
// ─────────────────────────────────────────────
void FFoxUtilsEditorModule::HandleListNodes(const TArray<FString>& Args)
{
	TArray<FFoxCategoryEntry> Catalog = BuildCatalog();
	int32 Total = 0;

	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("========== FoxUtils Node Catalog =========="));

	for (const FFoxCategoryEntry& Cat : Catalog)
	{
		UE_LOG(LogTemp, Log, TEXT(""));
		UE_LOG(LogTemp, Log, TEXT("--- %s (%d nodes) ---"), *Cat.CategoryName, Cat.Functions.Num());
		for (const FFoxNodeEntry& F : Cat.Functions)
		{
			UFunction* Func = Cat.OwnerClass->FindFunctionByName(F.FunctionName);
			if (Func)
			{
				bool bPure = Func->HasAnyFunctionFlags(FUNC_BlueprintPure);
				UE_LOG(LogTemp, Log, TEXT("  [%s] %s::%s"),
					bPure ? TEXT("Pure") : TEXT("Call"),
					*Cat.OwnerClass->GetName(),
					*F.FunctionName.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  [????] %s (not found!)"), *F.FunctionName.ToString());
			}
			Total++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("Total: %d nodes in %d categories"), Total, Catalog.Num());
	UE_LOG(LogTemp, Log, TEXT("==========================================="));
}

// ─────────────────────────────────────────────
// FoxDebug.SystemInfo
// ─────────────────────────────────────────────
void FFoxUtilsEditorModule::HandleSystemInfo(const TArray<FString>& Args)
{
	FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
	FTextureMemoryStats TexMemStats;
	RHIGetTextureMemoryStats(TexMemStats);

	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("========== FoxDebug System Info =========="));
	UE_LOG(LogTemp, Log, TEXT("  CPU:       %s"), *FPlatformMisc::GetCPUBrand());
	UE_LOG(LogTemp, Log, TEXT("  CPU Cores: %d physical, %d logical"), FPlatformMisc::NumberOfCores(), FPlatformMisc::NumberOfCoresIncludingHyperthreads());
	UE_LOG(LogTemp, Log, TEXT("  GPU:       %s"), *GRHIAdapterName);
	UE_LOG(LogTemp, Log, TEXT("  RAM:       %llu MB"), MemStats.TotalPhysical / (1024 * 1024));
	UE_LOG(LogTemp, Log, TEXT("  RAM Used:  %llu MB"), (MemStats.TotalPhysical - MemStats.AvailablePhysical) / (1024 * 1024));
	if (TexMemStats.DedicatedVideoMemory > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("  VRAM:      %lld MB"), TexMemStats.DedicatedVideoMemory / (1024 * 1024));
	}
	UE_LOG(LogTemp, Log, TEXT("  Platform:  %s"), *FString(FPlatformProperties::PlatformName()));
	UE_LOG(LogTemp, Log, TEXT("  Engine:    %s"), *FEngineVersion::Current().ToString());
	UE_LOG(LogTemp, Log, TEXT("=========================================="));
}

// ─────────────────────────────────────────────
// FoxDebug.CountActors
// ─────────────────────────────────────────────
void FFoxUtilsEditorModule::HandleCountActors(const TArray<FString>& Args)
{
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("FoxDebug: No world available"));
		return;
	}

	FString Filter = Args.Num() > 0 ? Args[0] : TEXT("");
	TMap<FString, int32> ClassCounts;
	int32 Total = 0;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		FString ClassName = Actor->GetClass()->GetName();

		if (Filter.IsEmpty() || ClassName.Contains(Filter))
		{
			ClassCounts.FindOrAdd(ClassName)++;
			Total++;
		}
	}

	// Sort by count descending
	ClassCounts.ValueSort([](int32 A, int32 B) { return A > B; });

	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("========== FoxDebug Actor Count =========="));
	if (!Filter.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("  Filter: '%s'"), *Filter);
	}
	for (const auto& Pair : ClassCounts)
	{
		UE_LOG(LogTemp, Log, TEXT("  %4d x %s"), Pair.Value, *Pair.Key);
	}
	UE_LOG(LogTemp, Log, TEXT("  ----"));
	UE_LOG(LogTemp, Log, TEXT("  Total: %d actors"), Total);
	UE_LOG(LogTemp, Log, TEXT("=========================================="));
}

// ─────────────────────────────────────────────
// FoxDebug.TraceDebug
// ─────────────────────────────────────────────
void FFoxUtilsEditorModule::HandleTraceDebug(const TArray<FString>& Args)
{
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("FoxDebug: No world available"));
		return;
	}

	// Get editor viewport camera
	FVector CamLoc;
	FRotator CamRot;
	if (GEditor && GEditor->GetActiveViewport())
	{
		FEditorViewportClient* Client = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
		if (Client)
		{
			CamLoc = Client->GetViewLocation();
			CamRot = Client->GetViewRotation();
		}
	}

	float TraceLength = (Args.Num() > 0) ? FCString::Atof(*Args[0]) : 10000.f;
	FVector End = CamLoc + CamRot.Vector() * TraceLength;

	FHitResult Hit;
	FCollisionQueryParams Params;

	bool bHit = World->LineTraceSingleByChannel(Hit, CamLoc, End, ECC_Visibility, Params);

	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("========== FoxDebug Trace =========="));
	UE_LOG(LogTemp, Log, TEXT("  From:     %s"), *CamLoc.ToString());
	UE_LOG(LogTemp, Log, TEXT("  Dir:      %s"), *CamRot.Vector().ToString());
	UE_LOG(LogTemp, Log, TEXT("  Distance: %.0f"), TraceLength);

	if (bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("  HIT!"));
		UE_LOG(LogTemp, Log, TEXT("  Actor:    %s"), Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("None"));
		UE_LOG(LogTemp, Log, TEXT("  Component:%s"), Hit.GetComponent() ? *Hit.GetComponent()->GetName() : TEXT("None"));
		UE_LOG(LogTemp, Log, TEXT("  Location: %s"), *Hit.ImpactPoint.ToString());
		UE_LOG(LogTemp, Log, TEXT("  Normal:   %s"), *Hit.ImpactNormal.ToString());
		UE_LOG(LogTemp, Log, TEXT("  Distance: %.1f"), Hit.Distance);
		UE_LOG(LogTemp, Log, TEXT("  PhysMat:  %s"), Hit.PhysMaterial.IsValid() ? *Hit.PhysMaterial->GetName() : TEXT("None"));

		// Draw debug line + point
		DrawDebugLine(World, CamLoc, Hit.ImpactPoint, FColor::Green, false, 5.f, 0, 2.f);
		DrawDebugPoint(World, Hit.ImpactPoint, 20.f, FColor::Red, false, 5.f);
		DrawDebugDirectionalArrow(World, Hit.ImpactPoint, Hit.ImpactPoint + Hit.ImpactNormal * 100.f, 20.f, FColor::Blue, false, 5.f, 0, 2.f);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("  No hit"));
		DrawDebugLine(World, CamLoc, End, FColor::Red, false, 5.f, 0, 1.f);
	}
	UE_LOG(LogTemp, Log, TEXT("===================================="));
}

// ─────────────────────────────────────────────
// FoxUtils.RunTests
// ─────────────────────────────────────────────
void FFoxUtilsEditorModule::HandleRunTests(const TArray<FString>& Args)
{
	int32 Passed = 0;
	int32 Failed = 0;
	int32 Total = 0;

	auto LogTest = [&](const FString& Category, const FString& Name, bool bSuccess, const FString& Detail)
	{
		Total++;
		if (bSuccess) { Passed++; } else { Failed++; }
		UE_LOG(LogTemp, Log, TEXT("  [%s] %s::%s — %s"),
			bSuccess ? TEXT("PASS") : TEXT("FAIL"),
			*Category, *Name, *Detail);
	};

	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("╔══════════════════════════════════════════╗"));
	UE_LOG(LogTemp, Log, TEXT("║       FoxUtils Automated Tests          ║"));
	UE_LOG(LogTemp, Log, TEXT("╚══════════════════════════════════════════╝"));

	// ══════════════════════════════════════════
	// STRING TESTS
	// ══════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── String ──"));
	{
		FString R = UFoxStringLibrary::TruncateString(TEXT("Hello World Test"), 8);
		LogTest(TEXT("String"), TEXT("TruncateString"), R == TEXT("Hello Wo..."), FString::Printf(TEXT("'%s'"), *R));
	}
	{
		bool R = UFoxStringLibrary::ContainsIgnoreCase(TEXT("Hello World"), TEXT("hello"));
		LogTest(TEXT("String"), TEXT("ContainsIgnoreCase"), R == true, R ? TEXT("true") : TEXT("false"));
	}
	{
		FString R = UFoxStringLibrary::ToTitleCase(TEXT("hello world test"));
		LogTest(TEXT("String"), TEXT("ToTitleCase"), R == TEXT("Hello World Test"), FString::Printf(TEXT("'%s'"), *R));
	}
	{
		bool R1 = UFoxStringLibrary::IsEmptyOrWhitespace(TEXT("   "));
		bool R2 = UFoxStringLibrary::IsEmptyOrWhitespace(TEXT("abc"));
		LogTest(TEXT("String"), TEXT("IsEmptyOrWhitespace"), R1 && !R2, FString::Printf(TEXT("spaces=%s, abc=%s"), R1 ? TEXT("true") : TEXT("false"), R2 ? TEXT("true") : TEXT("false")));
	}
	{
		FString R = UFoxStringLibrary::FormatNumber(1234567);
		LogTest(TEXT("String"), TEXT("FormatNumber"), R.Contains(TEXT(",")), FString::Printf(TEXT("'%s'"), *R));
	}
	{
		FString R = UFoxStringLibrary::FormatTimeSeconds(3661.5f, true, false);
		LogTest(TEXT("String"), TEXT("FormatTimeSeconds"), R.Len() > 0, FString::Printf(TEXT("'%s'"), *R));
	}
	{
		FString R = UFoxStringLibrary::RemoveWhitespace(TEXT("  h e l l o  "));
		LogTest(TEXT("String"), TEXT("RemoveWhitespace"), R == TEXT("hello"), FString::Printf(TEXT("'%s'"), *R));
	}
	{
		FString R = UFoxStringLibrary::PadString(TEXT("42"), 5, TEXT("0"), true);
		LogTest(TEXT("String"), TEXT("PadString"), R == TEXT("00042"), FString::Printf(TEXT("'%s'"), *R));
	}
	{
		FString L, Ri;
		bool R = UFoxStringLibrary::SplitFirst(TEXT("key=value"), TEXT("="), L, Ri);
		LogTest(TEXT("String"), TEXT("SplitFirst"), R && L == TEXT("key") && Ri == TEXT("value"), FString::Printf(TEXT("L='%s' R='%s'"), *L, *Ri));
	}
	{
		TArray<FString> Arr = { TEXT("A"), TEXT("B"), TEXT("C") };
		FString R = UFoxStringLibrary::JoinStringArray(Arr, TEXT(", "));
		LogTest(TEXT("String"), TEXT("JoinStringArray"), R == TEXT("A, B, C"), FString::Printf(TEXT("'%s'"), *R));
	}

	// ══════════════════════════════════════════
	// ARRAY TESTS
	// ══════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Array ──"));
	{
		TArray<int32> In = { 1, 2, 3, 4, 5 };
		TArray<int32> R = UFoxArrayLibrary::ShuffleIntArray(In);
		LogTest(TEXT("Array"), TEXT("ShuffleIntArray"), R.Num() == 5, FString::Printf(TEXT("Count=%d"), R.Num()));
	}
	{
		TArray<FString> In = { TEXT("A"), TEXT("B"), TEXT("C") };
		TArray<FString> R = UFoxArrayLibrary::ShuffleStringArray(In);
		LogTest(TEXT("Array"), TEXT("ShuffleStringArray"), R.Num() == 3, FString::Printf(TEXT("Count=%d"), R.Num()));
	}
	{
		TArray<int32> In = { 1, 2, 2, 3, 3, 3 };
		TArray<int32> R = UFoxArrayLibrary::RemoveDuplicateInts(In);
		LogTest(TEXT("Array"), TEXT("RemoveDuplicateInts"), R.Num() == 3, FString::Printf(TEXT("Count=%d (expected 3)"), R.Num()));
	}
	{
		TArray<FString> In = { TEXT("A"), TEXT("B"), TEXT("A"), TEXT("C") };
		TArray<FString> R = UFoxArrayLibrary::RemoveDuplicateStrings(In);
		LogTest(TEXT("Array"), TEXT("RemoveDuplicateStrings"), R.Num() == 3, FString::Printf(TEXT("Count=%d (expected 3)"), R.Num()));
	}
	{
		TArray<int32> In = { 10, 20, 30 };
		bool bSuccess = false;
		int32 R = UFoxArrayLibrary::RandomIntElement(In, false, bSuccess);
		LogTest(TEXT("Array"), TEXT("RandomIntElement"), bSuccess && In.Contains(R), FString::Printf(TEXT("Value=%d Success=%s"), R, bSuccess ? TEXT("true") : TEXT("false")));
	}
	{
		TArray<int32> In = { 1, 2, 3, 4, 5 };
		TArray<int32> R = UFoxArrayLibrary::RandomIntElements(In, 3);
		LogTest(TEXT("Array"), TEXT("RandomIntElements"), R.Num() == 3, FString::Printf(TEXT("Count=%d (expected 3)"), R.Num()));
	}
	{
		TArray<int32> In = { 5, 1, 3, 2, 4 };
		TArray<int32> R = UFoxArrayLibrary::SortIntArray(In, false);
		LogTest(TEXT("Array"), TEXT("SortIntArray"), R.Num() == 5 && R[0] == 1 && R[4] == 5, FString::Printf(TEXT("[%d,%d,%d,%d,%d]"), R[0], R[1], R[2], R[3], R[4]));
	}
	{
		TArray<float> In = { 3.1f, 1.2f, 2.5f };
		TArray<float> R = UFoxArrayLibrary::SortFloatArray(In, false);
		LogTest(TEXT("Array"), TEXT("SortFloatArray"), R.Num() == 3 && R[0] < R[2], FString::Printf(TEXT("[%.1f,%.1f,%.1f]"), R[0], R[1], R[2]));
	}
	{
		TArray<FString> In = { TEXT("Banana"), TEXT("Apple"), TEXT("Cherry") };
		TArray<FString> R = UFoxArrayLibrary::SortStringArray(In, false);
		LogTest(TEXT("Array"), TEXT("SortStringArray"), R[0] == TEXT("Apple"), FString::Printf(TEXT("[%s,%s,%s]"), *R[0], *R[1], *R[2]));
	}
	{
		TArray<float> Weights = { 0.1f, 0.8f, 0.1f };
		int32 R = UFoxArrayLibrary::WeightedRandomIndex(Weights);
		LogTest(TEXT("Array"), TEXT("WeightedRandomIndex"), R >= 0 && R < 3, FString::Printf(TEXT("Index=%d"), R));
	}

	// ══════════════════════════════════════════
	// MATH TESTS
	// ══════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Math ──"));
	{
		float R = UFoxMathLibrary::RemapRange(0.5f, 0.f, 1.f, 0.f, 100.f);
		LogTest(TEXT("Math"), TEXT("RemapRange"), FMath::IsNearlyEqual(R, 50.f), FString::Printf(TEXT("%.2f (expected 50)"), R));
	}
	{
		float R = UFoxMathLibrary::RoundToDecimals(3.14159f, 2);
		LogTest(TEXT("Math"), TEXT("RoundToDecimals"), FMath::IsNearlyEqual(R, 3.14f, 0.01f), FString::Printf(TEXT("%.2f (expected 3.14)"), R));
	}
	{
		float R = UFoxMathLibrary::NormalizeAngle(450.f, false);
		LogTest(TEXT("Math"), TEXT("NormalizeAngle"), FMath::IsNearlyEqual(R, 90.f, 0.1f), FString::Printf(TEXT("%.1f (expected 90)"), R));
	}
	{
		float R = UFoxMathLibrary::Distance2D(FVector(0, 0, 100), FVector(3, 4, 200));
		LogTest(TEXT("Math"), TEXT("Distance2D"), FMath::IsNearlyEqual(R, 5.f, 0.1f), FString::Printf(TEXT("%.1f (expected 5)"), R));
	}
	{
		bool R = UFoxMathLibrary::IsNearlyEqualFloat(1.0f, 1.0001f, 0.001f);
		LogTest(TEXT("Math"), TEXT("IsNearlyEqualFloat"), R == true, R ? TEXT("true") : TEXT("false"));
	}
	{
		int32 R1 = UFoxMathLibrary::SignFloat(42.f);
		int32 R2 = UFoxMathLibrary::SignFloat(-7.f);
		int32 R3 = UFoxMathLibrary::SignFloat(0.f);
		LogTest(TEXT("Math"), TEXT("SignFloat"), R1 == 1 && R2 == -1 && R3 == 0, FString::Printf(TEXT("+=%d -=%d 0=%d"), R1, R2, R3));
	}
	{
		float R = UFoxMathLibrary::WrapFloat(15.f, 0.f, 10.f);
		LogTest(TEXT("Math"), TEXT("WrapFloat"), R >= 0.f && R <= 10.f, FString::Printf(TEXT("%.1f"), R));
	}
	{
		float R = UFoxMathLibrary::Percentage(25.f, 200.f);
		LogTest(TEXT("Math"), TEXT("Percentage"), FMath::IsNearlyEqual(R, 12.5f, 0.1f), FString::Printf(TEXT("%.1f%% (expected 12.5)"), R));
	}

	// ══════════════════════════════════════════
	// PLATFORM TESTS
	// ══════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Platform ──"));
	{
		bool R = UFoxPlatformLibrary::IsPackagedBuild();
		LogTest(TEXT("Platform"), TEXT("IsPackagedBuild"), R == false, R ? TEXT("true (unexpected in editor!)") : TEXT("false (correct, running in editor)"));
	}
	{
		FString R = UFoxPlatformLibrary::GetEngineVersionString();
		LogTest(TEXT("Platform"), TEXT("GetEngineVersionString"), R.Len() > 0, FString::Printf(TEXT("'%s'"), *R));
	}
	{
		FString R = UFoxPlatformLibrary::GetPlatformName();
		LogTest(TEXT("Platform"), TEXT("GetPlatformName"), R.Len() > 0, FString::Printf(TEXT("'%s'"), *R));
	}
	{
		FString R = UFoxPlatformLibrary::GetGPUName();
		LogTest(TEXT("Platform"), TEXT("GetGPUName"), R.Len() > 0, FString::Printf(TEXT("'%s'"), *R));
	}
	{
		int32 R = UFoxPlatformLibrary::GetTotalRAM_MB();
		LogTest(TEXT("Platform"), TEXT("GetTotalRAM_MB"), R > 0, FString::Printf(TEXT("%d MB"), R));
	}
	{
		int32 R = UFoxPlatformLibrary::GetTotalVRAM_MB();
		LogTest(TEXT("Platform"), TEXT("GetTotalVRAM_MB"), R > 0, FString::Printf(TEXT("%d MB"), R));
	}
	{
		FString R = UFoxPlatformLibrary::GetCPUBrand();
		LogTest(TEXT("Platform"), TEXT("GetCPUBrand"), R.Len() > 0, FString::Printf(TEXT("'%s'"), *R));
	}
	{
		FIntPoint R = UFoxPlatformLibrary::GetDesktopResolution();
		LogTest(TEXT("Platform"), TEXT("GetDesktopResolution"), R.X > 0 && R.Y > 0, FString::Printf(TEXT("%dx%d"), R.X, R.Y));
	}

	// ══════════════════════════════════════════
	// SAVE TESTS
	// ══════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Save ──"));
	{
		// Write all values first, then verify by reading the INI file directly
		FString TestINIPath = FPaths::ProjectSavedDir() / TEXT("Config") / TEXT("FoxUtilsTest.ini");

		UFoxSaveLibrary::SaveStringToINI(TEXT("TestSection"), TEXT("StringKey"), TEXT("FoxValue"), TEXT("FoxUtilsTest"));
		UFoxSaveLibrary::SaveIntToINI(TEXT("TestSection"), TEXT("IntKey"), 42, TEXT("FoxUtilsTest"));
		UFoxSaveLibrary::SaveFloatToINI(TEXT("TestSection"), TEXT("FloatKey"), 3.14f, TEXT("FoxUtilsTest"));

		// Verify file was written with content
		FString FileContent;
		bool bFileExists = FFileHelper::LoadFileToString(FileContent, *TestINIPath);

		LogTest(TEXT("Save"), TEXT("SaveStringToINI"), bFileExists && FileContent.Contains(TEXT("FoxValue")),
			bFileExists ? FString::Printf(TEXT("File written (%d bytes)"), FileContent.Len()) : TEXT("File not created!"));
		LogTest(TEXT("Save"), TEXT("SaveIntToINI"), bFileExists && FileContent.Contains(TEXT("42")),
			bFileExists ? TEXT("Value '42' found in file") : TEXT("File not created!"));
		LogTest(TEXT("Save"), TEXT("SaveFloatToINI"), bFileExists && FileContent.Contains(TEXT("3.14")),
			bFileExists ? TEXT("Value '3.14' found in file") : TEXT("File not created!"));
	}

	// ══════════════════════════════════════════
	// INVENTORY TESTS
	// ══════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Inventory ──"));
	{
		TArray<int32> R = UFoxInventoryLibrary::CalculateStackSplit(10, 3);
		int32 Sum = 0;
		for (int32 V : R) { Sum += V; }
		LogTest(TEXT("Inventory"), TEXT("CalculateStackSplit"), R.Num() == 3 && Sum == 10, FString::Printf(TEXT("[%d,%d,%d] Sum=%d"), R[0], R[1], R[2], Sum));
	}
	{
		bool R1 = UFoxInventoryLibrary::CanStackItems(FName("Sword"), 5, FName("Sword"), 3, 10);
		bool R2 = UFoxInventoryLibrary::CanStackItems(FName("Sword"), 5, FName("Shield"), 3, 10);
		bool R3 = UFoxInventoryLibrary::CanStackItems(FName("Sword"), 8, FName("Sword"), 5, 10);
		LogTest(TEXT("Inventory"), TEXT("CanStackItems"), R1 && !R2 && !R3, FString::Printf(TEXT("same=%s diff=%s overflow=%s"), R1 ? TEXT("T") : TEXT("F"), R2 ? TEXT("T") : TEXT("F"), R3 ? TEXT("T") : TEXT("F")));
	}
	{
		TArray<FFoxWeightEntry> Items;
		FFoxWeightEntry E1; E1.Weight = 2.5f; E1.Count = 4; Items.Add(E1);
		FFoxWeightEntry E2; E2.Weight = 1.0f; E2.Count = 3; Items.Add(E2);
		float R = UFoxInventoryLibrary::CalculateTotalWeight(Items);
		LogTest(TEXT("Inventory"), TEXT("CalculateTotalWeight"), FMath::IsNearlyEqual(R, 13.f, 0.1f), FString::Printf(TEXT("%.1f (expected 13)"), R));
	}
	{
		bool R1 = UFoxInventoryLibrary::FitsInContainer(2, 10, 7);
		bool R2 = UFoxInventoryLibrary::FitsInContainer(2, 10, 9);
		LogTest(TEXT("Inventory"), TEXT("FitsInContainer"), R1 && !R2, FString::Printf(TEXT("fits=%s overflow=%s"), R1 ? TEXT("T") : TEXT("F"), R2 ? TEXT("T") : TEXT("F")));
	}
	{
		// 4x4 grid, first 2 rows occupied
		TArray<bool> Grid;
		Grid.Init(false, 16);
		for (int32 i = 0; i < 8; i++) { Grid[i] = true; }
		int32 X = -1, Y = -1;
		bool R = UFoxInventoryLibrary::FindBestFitSlot(Grid, 4, 4, 2, 1, X, Y);
		LogTest(TEXT("Inventory"), TEXT("FindBestFitSlot"), R && Y >= 2, FString::Printf(TEXT("Found=%s X=%d Y=%d"), R ? TEXT("T") : TEXT("F"), X, Y));
	}

	// ══════════════════════════════════════════
	// DEBUG TESTS (non-world only)
	// ══════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("── Debug ──"));
	{
		UFoxDebugLibrary::FoxLog(TEXT("FoxUtils.RunTests test message"), TEXT("TestCat"), false);
		LogTest(TEXT("Debug"), TEXT("FoxLog"), true, TEXT("Logged without crash"));
	}

	// ══════════════════════════════════════════
	// SUMMARY
	// ══════════════════════════════════════════
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("╔══════════════════════════════════════════╗"));
	UE_LOG(LogTemp, Log, TEXT("║  Results: %d/%d passed  %s"), Passed, Total,
		Failed == 0 ? TEXT("  ALL CLEAR! ║") : TEXT("            ║"));
	if (Failed > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("║  FAILED: %d tests                       ║"), Failed);
	}
	UE_LOG(LogTemp, Log, TEXT("╚══════════════════════════════════════════╝"));
	UE_LOG(LogTemp, Log, TEXT(""));
	UE_LOG(LogTemp, Log, TEXT("Note: World-dependent nodes (Actor, Trace, Camera, Loop, Audio, InstancedMesh) skipped."));
	UE_LOG(LogTemp, Log, TEXT("Use FoxUtils.CreateTestRunner for world tests (Play in Editor)."));
}

// ─────────────────────────────────────────────
// FoxUtils.CreateTestRunner
// ─────────────────────────────────────────────
void FFoxUtilsEditorModule::HandleCreateTestRunner(const TArray<FString>& Args)
{
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("FoxUtils: No world available. Open a level first."));
		return;
	}

	// Check if a TestRunner already exists
	for (TActorIterator<AFoxUtilsTestRunner> It(World); It; ++It)
	{
		UE_LOG(LogTemp, Warning, TEXT("FoxUtils: TestRunner already exists in level (%s). Removing old one..."), *(*It)->GetName());
		(*It)->Destroy();
	}

	// Spawn at world origin with some height
	FVector SpawnLoc = FVector(0.f, 0.f, 200.f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = FName(TEXT("FoxUtilsTestRunner"));

	AFoxUtilsTestRunner* Runner = World->SpawnActor<AFoxUtilsTestRunner>(AFoxUtilsTestRunner::StaticClass(), SpawnLoc, FRotator::ZeroRotator, SpawnParams);

	if (Runner)
	{
		UE_LOG(LogTemp, Log, TEXT(""));
		UE_LOG(LogTemp, Log, TEXT("=== FoxUtils TestRunner Spawned! ==="));
		UE_LOG(LogTemp, Log, TEXT("  Location: %s"), *SpawnLoc.ToString());
		UE_LOG(LogTemp, Log, TEXT("  Press PLAY to run world-dependent tests."));
		UE_LOG(LogTemp, Log, TEXT("  Tests: Actor, Trace, Camera, Debug (world)"));
		UE_LOG(LogTemp, Log, TEXT("===================================="));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FoxUtils: Failed to spawn TestRunner actor!"));
	}
}
