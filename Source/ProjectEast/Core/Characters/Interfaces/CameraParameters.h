#pragma once

#include "CoreMinimal.h"
#include "ProjectEast/Core/Utils/Structurs.h"
#include "UObject/Interface.h"
#include "CameraParameters.generated.h"

UINTERFACE(MinimalAPI)
class UCameraParameters : public UInterface
{
	GENERATED_BODY()
};


class PROJECTEAST_API ICameraParameters
{
	GENERATED_BODY()

public:
	virtual FCameraViewParameters GetCameraParameters();
	virtual FVector GetFPCameraTarget();
	virtual FTransform GetTPPivotTarget();
	virtual FTPTraceParams GetTPTraceParams();
};
