#include "AbilitySystem/Abilities/AuraArcaneShards.h"
#include "Actor/AuraAreaAttack.h"
#include "GameFramework/Pawn.h"

AAuraAreaAttack* UAuraArcaneShards::SpawnAreaAttack(const FVector& SpawnLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority() || !AreaAttackClass)
	{
		return nullptr;
	}

	const FTransform SpawnTransform(GetAvatarActorFromActorInfo()->GetActorRotation(), SpawnLocation);
	AAuraAreaAttack* AreaAttack = GetWorld()->SpawnActorDeferred<AAuraAreaAttack>(AreaAttackClass,SpawnTransform,GetOwningActorFromActorInfo(),Cast<APawn>(GetOwningActorFromActorInfo()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	AreaAttack->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	AreaAttack->FinishSpawning(SpawnTransform);
	return AreaAttack;
}
