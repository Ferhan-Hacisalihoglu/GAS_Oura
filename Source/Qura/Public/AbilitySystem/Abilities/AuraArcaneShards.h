#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraArcaneShards.generated.h"

class AAuraAreaAttack;

UCLASS()
class QURA_API UAuraArcaneShards : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AAuraAreaAttack* SpawnAreaAttack(const FVector& SpawnLocation);

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraAreaAttack> AreaAttackClass;
};
