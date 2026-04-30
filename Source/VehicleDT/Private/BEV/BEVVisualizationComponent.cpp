// Fill out your copyright notice in the Description page of Project Settings.

#include "BEV/BEVVisualizationComponent.h"

UBEVVisualizationComponent::UBEVVisualizationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBEVVisualizationComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateTexture();
	BuildColorLUT();
}

void UBEVVisualizationComponent::HandleLidarScan(const FLidarPointCloudData& PointCloud)
{
	UpdatePixelBuffer(PointCloud);
}

void UBEVVisualizationComponent::CreateTexture()
{
}

void UBEVVisualizationComponent::BuildColorLUT()
{
}

void UBEVVisualizationComponent::UpdatePixelBuffer(const FLidarPointCloudData& PointCloud)
{
	if (bShowGrid)    DrawGrid();
	if (bShowHeading) DrawHeading();
}

void UBEVVisualizationComponent::DrawGrid()
{
	OnBEVUpdate.Broadcast(DynamicTexture);
}

void UBEVVisualizationComponent::DrawHeading()
{
}

FColor UBEVVisualizationComponent::MakeColorFromHeight(float Height) const
{
	return FColor::White;
}
