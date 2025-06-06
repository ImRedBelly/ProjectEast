﻿#include "TraceUtils.h"

bool TraceUtils::SweepCapsuleSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start,
                                            const FVector& End, float CapsuleRadius, float CapsuleHalfHeight,
                                            const FQuat& Rot, ECollisionChannel TraceChannel,
                                            const FCollisionQueryParams& Params,
                                            const FCollisionResponseParams& ResponseParam, bool bDrawDebug,
                                            float DrawTime,
                                            FColor TraceColor, FColor HitColor)
{
	bool bResult = false;

	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	bResult = World->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CollisionShape, Params, ResponseParam);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugCapsule(World, Start, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, TraceColor, false, DrawTime);
		DrawDebugCapsule(World, End, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, TraceColor, false, DrawTime);
		DrawDebugLine(World, Start, End, TraceColor, false, DrawTime);

		if (bResult)
		{
			DrawDebugCapsule(World, OutHit.Location, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, HitColor, false,
			                 DrawTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10.0f, HitColor, false, DrawTime);
		}
	}
#endif
	return bResult;
}

bool TraceUtils::SweepSphereSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start,
                                           const FVector& End, float Radius, ECollisionChannel TraceChannel,
                                           const FCollisionQueryParams& Params,
                                           const FCollisionResponseParams& ResponseParam, bool bDrawDebug,
                                           float DrawTime, FColor TraceColor, FColor HitColor)
{
	bool bResult = false;

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);
	bResult = World->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, CollisionShape, Params,
	                                      ResponseParam);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		FVector DebugCapsuleLocation = (Start + End) * 0.5f;
		FVector TraceVector = End - Start;
		float DebugDrawCapsuleHalfHeight = TraceVector.Size() * 0.5f;

		FQuat DebugCapsuleRotation = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();

		DrawDebugCapsule(World, DebugCapsuleLocation, DebugDrawCapsuleHalfHeight, Radius, DebugCapsuleRotation,
		                 TraceColor, false, DrawTime);;

		if (bDrawDebug)
		{
			DrawDebugSphere(World, OutHit.Location, Radius, 32, HitColor, false, DrawTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10.0f, HitColor, false, DrawTime);
		}
	}
#endif
	return bResult;
}

bool TraceUtils::OverlapAnyTestByProfile(const UWorld* World, const FVector& Position, float CapsuleRadius,
                                         float CapsuleHalfHeight, FQuat Rotation, FName ProfileName,
                                         FCollisionQueryParams Params, bool bDrawDebug,
                                         float DrawTime, FColor HitColor)
{
	bool bResult = false;

	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	bResult = World->OverlapAnyTestByProfile(Position, Rotation, ProfileName, CollisionShape, Params);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug && bResult)
	{
		DrawDebugCapsule(World, Position, CapsuleHalfHeight, CapsuleRadius, Rotation, HitColor,
		                 false, DrawTime);
	}
#endif
	return bResult;
}

bool TraceUtils::OverlapCapsuleBlockingByProfile(const UWorld* World, const FVector& Position, float CapsuleRadius,
                                                 float CapsuleHalfHeight, FQuat Rotation, FName ProfileName,
                                                 FCollisionQueryParams Params, bool bDrawDebug,
                                                 float DrawTime, FColor HitColor)
{
	bool bResult = false;

	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	bResult = World->OverlapBlockingTestByProfile(Position, Rotation, ProfileName, CollisionShape, Params);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug && bResult)
	{
		DrawDebugCapsule(World, Position, CapsuleHalfHeight, CapsuleRadius, Rotation, HitColor,
		                 false, DrawTime);
	}
#endif
	return bResult;
}
