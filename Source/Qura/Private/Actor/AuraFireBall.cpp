#include "Actor/AuraFireBall.h"


void AAuraFireBall::BeginPlay()
{
    Super::BeginPlay();   
    StartOutgoingTimeline();
}