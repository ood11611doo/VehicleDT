// Fill out your copyright notice in the Description page of Project Settings.

#include "Sensor/SensorViewWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"

void USensorViewWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SensorBorder) SensorBorder->SetVisibility(ESlateVisibility::Collapsed);
	if (LidarBorder) LidarBorder->SetVisibility(ESlateVisibility::Collapsed);
}

void USensorViewWidget::SetRenderTarget(UTextureRenderTarget2D* InRenderTarget)
{
	if (SensorImage && InRenderTarget) SensorImage->SetBrushResourceObject(InRenderTarget);
}

void USensorViewWidget::SetLidarRenderTarget(UTexture2D* InRenderTarget)
{
	if (LidarImage && InRenderTarget) LidarImage->SetBrushResourceObject(InRenderTarget);
}

void USensorViewWidget::ToggleCameraView()
{
	if (!SensorBorder) return;
	SensorBorder->SetVisibility(IsCameraViewVisible() 
		? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

void USensorViewWidget::ToggleLidarView()
{
	if (!LidarBorder) return;
	LidarBorder->SetVisibility(IsLidarViewVisible() 
		? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

bool USensorViewWidget::IsCameraViewVisible() const
{
	return SensorBorder && SensorBorder->GetVisibility() == ESlateVisibility::Visible;
}

bool USensorViewWidget::IsLidarViewVisible() const
{
	return LidarBorder && LidarBorder->GetVisibility() == ESlateVisibility::Visible;
}
