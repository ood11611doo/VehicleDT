#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AgentDataLogger.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VEHICLEDT_API UAgentDataLogger : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAgentDataLogger();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
