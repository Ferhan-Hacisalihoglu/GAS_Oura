#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
} 

void UAuraAbilitySystemComponent::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLoc(*this);

    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
        {
            if (!AbilitySpec.IsActive())
            {
                TryActivateAbility(AbilitySpec.Handle);
            }
        }
    }
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
        if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
            AbilitySpec.GetDynamicSpecSourceTags().AddTag(FGameplayTag::RequestGameplayTag("Abilities.Status.Equipped"));
            GiveAbility(AbilitySpec);
        }
    }
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
        GiveAbilityAndActivateOnce(AbilitySpec);
    }
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (FGameplayTag StatusTag : AbilitySpec.GetDynamicSpecSourceTags())
    {
        if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
        {
            return StatusTag;
        }
    }
    return FGameplayTag();
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveHandle)
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
    FScopedAbilityListLock AbilityListLock(*this);
    for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        Delegate.ExecuteIfBound(AbilitySpec);
    }
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    if(AbilitySpec.Ability)
    {
        for (FGameplayTag Tag : AbilitySpec.Ability.Get()->GetAssetTags())
        {
            if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Abilities")))
            {
                return Tag;
            }
        }
    }
    return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
    FScopedAbilityListLock AbilityListLock(*this);
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        for (FGameplayTag Tag : AbilitySpec.Ability.Get()->GetAssetTags())
        {
            if(Tag.MatchesTag(AbilityTag))
            {
                return &AbilitySpec;
            }
        }
    }
    return nullptr;
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
    FScopedAbilityListLock AbilityListLock(*this);
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if(AbilityHasSlot(AbilitySpec, Slot))
        {
            return false;
        }
    }
    return true;
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
    return Spec.GetDynamicSpecSourceTags().HasTagExact(Slot);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
    FScopedAbilityListLock AbilityListLock(*this);
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(Slot))
        {
            return &AbilitySpec;
        }
    }
    return nullptr;
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
    if(FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
    {
        const FGameplayTag PrevSlot = GetInputTagFromSpec(*AbilitySpec);
        const FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
        const bool bStatusValid = Status.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"))) || Status.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked")));

        if(bStatusValid)
        {
            if(!SlotIsEmpty(Slot))
            {
                FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);

                if(SpecWithSlot)
                {
                    if(AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
                    {
                        ClientEquipAbility(AbilityTag, FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped")),Slot, PrevSlot);
                        return;
                    }

                    if(IsPassiveAbility(*SpecWithSlot))
                    {
                        MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
                    }

                    const FGameplayTag RemoveSlot = GetInputTagFromSpec(*SpecWithSlot);
                    SpecWithSlot->GetDynamicSpecSourceTags().RemoveTag(RemoveSlot);
                }
            }

            if (!AbilityHasAnySlot(*AbilitySpec))
            {
                if(IsPassiveAbility(*AbilitySpec))
                {
                    TryActivateAbility(AbilitySpec->Handle);
                    MulticastActivatePassiveEffect(AbilityTag, true);
                }

                AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GetStatusFromSpec(*AbilitySpec));
                AbilitySpec->GetDynamicSpecSourceTags().AddTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped")));
            }

            AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(PrevSlot);
            AbilitySpec->GetDynamicSpecSourceTags().AddTag(Slot);
            MarkAbilitySpecDirty(*AbilitySpec);
        }

        ClientEquipAbility(AbilityTag, FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped")),Slot, PrevSlot);

    }
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
    return Spec.GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
    const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
    const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
    const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
    const FGameplayTag AbilityType = Info.AbilityType;
    return AbilityType.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Type.Passive")));
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, bool bActivate)
{
    ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
    UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
    for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
    {
        if (!Info.AbilityTag.IsValid()) continue;

        if(GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
        {
            FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
            AbilitySpec.GetDynamicSpecSourceTags().AddTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible")));
            GiveAbility(AbilitySpec);
            MarkAbilitySpecDirty(AbilitySpec);
            ClientUpdateAbilityStatuses(Info.AbilityTag,FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible")),1);
        }
    }
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
    if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);

		if (Status.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible"))))
		{
            AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Eligible")));
            AbilitySpec->GetDynamicSpecSourceTags().AddTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked")));
			Status = FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"));
		}
		else if (Status.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Equipped"))) || Status.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Abilities.Status.Unlocked"))))
		{
			AbilitySpec->Level += 1;
		}
		ClientUpdateAbilityStatuses(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatuses_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
    AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
    AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}