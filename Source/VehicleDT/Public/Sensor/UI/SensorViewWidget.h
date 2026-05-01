#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SensorViewWidget.generated.h"

class UImage;
class UBorder;
class UTextureRenderTarget2D;

UCLASS()
class VEHICLEDT_API USensorViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SensorWidget")
	void SetRenderTarget(UTextureRenderTarget2D* InTarget);

	UFUNCTION(BlueprintCallable, Category = "SensorWidget")
	void SetLidarTexture(UTexture2D* InTexture);

	UFUNCTION(BlueprintCallable, Category = "SensorWidget")
	void ToggleCameraView();

	UFUNCTION(BlueprintCallable, Category = "SensorWidget")
	void ToggleLidarView();

	UFUNCTION(BlueprintPure, Category = "SensorWidget")
	bool IsCameraVisible() const;

	UFUNCTION(BlueprintPure, Category = "SensorWidget")
	bool IsLidarVisible() const;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CameraPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CameraImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> LidarPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LidarImage;
};
