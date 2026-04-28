#include "DataLogger/AgentDataLogger.h"

UAgentDataLogger::UAgentDataLogger()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UAgentDataLogger::BeginPlay()
{
	Super::BeginPlay();
}

void UAgentDataLogger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UAgentDataLogger::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
