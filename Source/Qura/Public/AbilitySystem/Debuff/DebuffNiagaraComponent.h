#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "DebuffNiagaraComponent.generated.h"

UCLASS()
class QURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UDebuffNiagaraComponent();

	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;

protected:
	virtual void BeginPlay() override;
	void DebuffTagChanged(const FGameplayTag DebuffTag, int32 NewCount);
	
};
