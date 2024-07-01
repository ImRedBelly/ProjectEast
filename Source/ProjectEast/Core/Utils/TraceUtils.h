#pragma once

namespace TraceUtils
{

	bool SweepCapsuleSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start,
	                                const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot,
	                                ECollisionChannel TraceChannel,
	                                const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
	                                const FCollisionResponseParams& ResponseParam =
		                                FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false,
	                                float DrawTime = -1, FColor TraceColor = FColor::Black,
	                                FColor HitColor = FColor::Red);

	bool SweepSphereSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start,
	                               const FVector& End, float Radius, ECollisionChannel TraceChannel,
	                               const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
	                               const FCollisionResponseParams& ResponseParam =
		                               FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false,
	                               float DrawTime = -1, FColor TraceColor = FColor::Black,
	                               FColor HitColor = FColor::Red);

	bool OverlapAnyTestByProfile(const UWorld* World, const FVector& Position, float CapsuleRadius,
	                             float CapsuleHalfHeight,
	                             FQuat Rotation, FName ProfileName, FCollisionQueryParams Params,
	                             bool bDrawDebug = false,
	                             float DrawTime = -1, FColor HitColor = FColor::Red);
	bool OverlapCapsuleBlockingByProfile(const UWorld* World, const FVector& Position, float CapsuleRadius,
	                                     float CapsuleHalfHeight,
	                                     FQuat Rotation, FName ProfileName, FCollisionQueryParams Params,
	                                     bool bDrawDebug = false,
	                                     float DrawTime = -1, FColor HitColor = FColor::Red);
};
