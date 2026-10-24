#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
    static AuraDamageStatics DStatics;
    return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);

    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);

}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> InTagsToDefs) const
{
    const float SourceDebuffChange = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Debuff.Chance"), false, -1.0f);
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%f"), SourceDebuffChange));

    FGameplayTag ResistanceTag = UAuraAbilitySystemLibrary::GetDamageResistanceType(Spec.GetContext());
    if (!ResistanceTag.IsValid())
    {
        for (const auto& Pair : InTagsToDefs)
        {
            if (Spec.SetByCallerTagMagnitudes.Contains(Pair.Key))
            {
                ResistanceTag = Pair.Key;
                break;
            }
        }
    }

    const FGameplayTag DebuffType = UAuraAbilitySystemLibrary::GetDebuffType(Spec.GetContext());

    float TargetDebuffResistance = 0.0f;
    if (const FGameplayEffectAttributeCaptureDefinition* CaptureDef = InTagsToDefs.Find(ResistanceTag))
    {
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(*CaptureDef, EvaluationParameters, TargetDebuffResistance);
        TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.0f);
    }

    const float EffectiveDebuffChange = SourceDebuffChange * ( 100 - TargetDebuffResistance) / 100.0f;
    const bool bDebuff = FMath::RandRange(1,100) < EffectiveDebuffChange;

    if (bDebuff)
    {
        FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

        UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
        UAuraAbilitySystemLibrary::SetDebuffType(ContextHandle, DebuffType);

        const float DebuffDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Debuff.Damage"), false, -1.0f);
        const float DebuffDuration = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Debuff.Duration"), false, -1.0f);
        const float DebuffFrequency = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Debuff.Frequency"), false, -1.0f);

        UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
        UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
        UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
    }
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
    
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Secondary.Armor"), DamageStatics().ArmorDef);
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Secondary.BlockChance"), DamageStatics().BlockChanceDef);
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Secondary.ArmorPenetration"), DamageStatics().ArmorPenetrationDef);
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Secondary.CriticalHitChance"), DamageStatics().CriticalHitChanceDef);
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Secondary.CriticalHitResistance"), DamageStatics().CriticalHitResistanceDef);
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Secondary.CriticalHitDamage"), DamageStatics().CriticalHitDamageDef);

    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Resistance.Fire"), DamageStatics().FireResistanceDef);
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Resistance.Lightning"), DamageStatics().LightningResistanceDef);
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Resistance.Arcane"), DamageStatics().ArcaneResistanceDef);
    TagsToCaptureDefs.Add(FGameplayTag::RequestGameplayTag("Attributes.Resistance.Physical"), DamageStatics().PhysicalResistanceDef);

    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

    DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);

    float Damage = 41.0f;

    float TargetBlockChance = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.0f);

    const bool bBlocked = FMath::RandRange(1,100) < TargetBlockChance;

    UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

    Damage = bBlocked ? Damage / 2.0f : Damage;
    
    const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);

	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;

    const FGameplayModifierEvaluatedData EvaulatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaulatedData);
}