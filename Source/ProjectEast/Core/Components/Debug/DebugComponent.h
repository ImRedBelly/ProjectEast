// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DebugComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEAST_API UDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool GetShowTraces() { return bShowTraces; }

	static void DrawDebugLineTraceSingle(const UWorld* World,
	                                     const FVector& Start,
	                                     const FVector& End,
	                                     EDrawDebugTrace::Type DrawDebugType,
	                                     bool bHit,
	                                     const FHitResult& OutHit,
	                                     FLinearColor TraceColor,
	                                     FLinearColor TraceHitColor,
	                                     float DrawTime);

private:
	static bool bShowTraces;
};
