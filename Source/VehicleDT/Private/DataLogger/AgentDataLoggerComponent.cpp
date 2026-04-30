// Fill out your copyright notice in the Description page of Project Settings.

#include "DataLogger/AgentDataLoggerComponent.h"

UAgentDataLoggerComponent::UAgentDataLoggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAgentDataLoggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAgentDataLoggerComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (bIsRecording)
	{
		StopRecording();
	}
}

void UAgentDataLoggerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAgentDataLoggerComponent::StartRecording()
{
}

void UAgentDataLoggerComponent::HandleLidarScan(const FLidarPointCloudData& PointCloud)
{
}

void UAgentDataLoggerComponent::HandleCameraFrame()
{
}

void UAgentDataLoggerComponent::HandleControlOutput(float Value)
{
}

void UAgentDataLoggerComponent::StopRecording()
{
}

int32 UAgentDataLoggerComponent::GetUtmZone(double Longitude)
{
	return static_cast<int32>((Longitude + 180.0) / 6.0) + 1;
}

void UAgentDataLoggerComponent::LatLonToUtm(double Lat, double Lon, int32 Zone,
	double& OutEasting, double& OutNorthing)
{
}

void UAgentDataLoggerComponent::WorldToUtm(const FVector& WorldLocation,
	double& OutEasting, double& OutNorthing) const
{
}

void UAgentDataLoggerComponent::CreateCsvFile()
{
}

void UAgentDataLoggerComponent::AppendRow()
{
}
