#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"



UCLASS()
class QURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;

protected:
	virtual void BeginPlay() override;
};
