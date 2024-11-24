#include "CameraParameters.h"

FCameraViewParameters ICameraParameters::GetCameraParameters()
{
	return FCameraViewParameters();
}

FVector ICameraParameters::GetFPCameraTarget()
{
	return FVector();
}

FTransform ICameraParameters::GetTPPivotTarget()
{
	return FTransform();
}

FTPTraceParams ICameraParameters::GetTPTraceParams()
{
	return FTPTraceParams();
}
