#include "DebugComponent.h"

bool UDebugComponent::bShowTraces = false;
void UDebugComponent::DrawDebugLineTraceSingle(const UWorld* World,
                                               const FVector& Start,
                                               const FVector& End,
                                               EDrawDebugTrace::Type
                                               DrawDebugType,
                                               bool bHit,
                                               const FHitResult& OutHit,
                                               FLinearColor TraceColor,
                                               FLinearColor TraceHitColor,
                                               float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			DrawDebugLine(World, Start, OutHit.ImpactPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugLine(World, OutHit.ImpactPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 16.0f, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}
