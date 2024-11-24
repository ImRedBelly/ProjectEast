#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectEast/Core/Utils/GameTypes.h"
#include "MathLibrary.generated.h"

UCLASS()
class PROJECTEAST_API UMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static EMovementDirection CalculateQuadrant(EMovementDirection Current, float FRThreshold, float FLThreshold,
	                                            float BRThreshold,
	                                            float BLThreshold, float Buffer, float Angle);

	
	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool IncreaseBuffer);
};
