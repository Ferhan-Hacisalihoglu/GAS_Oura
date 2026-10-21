#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
    UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);

    const float DebuffDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Debuff.Damage"), false, -1.0f);
    const float DebuffDuration = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Debuff.Duration"), false, -1.0f);
    const float DebuffFrequency = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Debuff.Frequency"), false, -1.0f);

    UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
    UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
    UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);

    float Damage = 51.0f;
    const FGameplayModifierEvaluatedData EvaulatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaulatedData);
}