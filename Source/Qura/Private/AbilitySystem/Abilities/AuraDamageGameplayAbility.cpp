#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.DamageResistanceType = DamageResistanceType;
	Params.DamageDebuffType = DamageDebuffType;
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;

	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		const FVector ToTarget = Rotation.Vector();
		Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
	}
	
	return Params;
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& MontageArray) const
{
	if (MontageArray.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, MontageArray.Num() - 1);
		return MontageArray[RandomIndex];
	}

	return FTaggedMontage();
}
