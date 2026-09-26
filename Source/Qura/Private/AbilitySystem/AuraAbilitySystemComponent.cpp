#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
} 

void UAuraAbilitySystemComponent::AbilityInputTagHeld(FGameplayTag InputTag)
{
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (!AbilitySpec.IsActive())
        {
            TryActivateAbility(AbilitySpec.Handle);
        }
    }
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
        if(const UGameplayAbility* AuraAbility = AbilitySpec.Ability)
        {
            GiveAbility(AbilitySpec);
        }
    }
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveHandle)
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    EffectAssetTags.Broadcast(TagContainer);
}

