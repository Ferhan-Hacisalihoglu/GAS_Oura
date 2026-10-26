#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultsInfo UCharacterClassInfo::GetClassDefaultsInfo(ECharacterClass CharacterClass)
{
    return CharacterClassInformation.FindChecked(CharacterClass);
}