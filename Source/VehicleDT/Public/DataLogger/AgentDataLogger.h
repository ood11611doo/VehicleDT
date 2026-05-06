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

	//기록 시작
	UFUNCTION(BlueprintCallable, Category = "Data Logger")
	void StartRecording();
	
	//기록 중지
	UFUNCTION(BlueprintCallable, Category = "Data Logger")
	void StopRecording();
	
	//기록 중인지 확인하는 getter
	UFUNCTION(BlueprintPure, Category = "Data Logger")
	bool IsRecording() const { return bIsRecording; }
	
	//라이다센서에서 데이터 받기
	UFUNCTION()
	void HandleLidarScan(const TArray <FVector>& Points);
	
	// SplineFollower에서 입력값 받기
	UFUNCTION()
	void HandleControlOutput(const FVector2D& Input);
	
	// CameraSensor에서 프레임 준비 완료 신호 받기
	UFUNCTION()
	void HandleCameraFrameReady();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: // 함수
	//csv 관련 함수들 
	void CreateCsvFile(); // csv파일 만들고 해더 쓰기
	void AppendRow();	  // 매 0.1초마다 csv 한 줄 추가
	
	// UTM 관련 함수
	// 경도 값으로 UTM 구역 번호 계산
	static int32 GetUtmZone(double Longitude);
	
	// 위도/경도를 UTM 미터 좌표로 변환
	static void LatLonToUtm(double Lat, double Lon, int32 Zone,double& OutEasting, double& OutNorthing);
	
	// 게임 월드 좌표를 UTM 좌표로 변환
	void WorldToUtm(const FVector& WorldLocation, double& OutEasting, double& OutNorthing) const;
	
	// DrawDebufLine으로 주행 궤적 그리기
	void DrawDebugTrail();
	
private: // 변수
	//에디터 설정 값
	//로그 기록 활성화
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger", meta = (AllowPrivateAccess = "true"))
	bool bEnableLogging = true;
	
	//주파수(Hz) 저장
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger", meta = (AllowPrivateAccess = "true"))
	float SaveFrequencyHz = 10.f;
	
	//DrawDebug 시각화 켜고 끄기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger", meta = (AllowPrivateAccess = "true"))
	bool DebugDrawEnabled = true;
	
	// 급감속 판별 임계값 (체크리스트)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger", meta = (AllowPrivateAccess = "true"))
	float HardBrakeThreshold = 10.f;
	
	//DrawDebugString 표시 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger", meta = (AllowPrivateAccess = "true"))
	float DebugStringInterval = 500.f;
	
	//UTM 기준점 위도 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger", meta = (AllowPrivateAccess = "true"))
	double OriginLatitude = 36.4800;
	///UTM 기준점 경도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger", meta = (AllowPrivateAccess = "true"))
	double OriginLongitude = 127.0000;
	
private: // 내부 상태 값
	FString CSVFilePath;				// CSV 파일이 저장될 경로 (프로젝트/Output/AgentData ~ )
	bool bIsRecording = false;			// 기록 상태
	float TimeSinceLastSave = 0.f;	    // 마지막 저장 이후 경과 시간
	float ElapsedRecordingTime = 0.f;	// 기록 경과 시간
	
	double OriginUtmEasting = 0.0;	// 기준점의 UTM X좌표 (미터)
	double OriginUtmNorthing = 0.0;// 기준점의 UTM Y좌표 (미터)
	int32 OriginUtmZone = 0;		// UTM 구역 번호 (한국은 UTM 52번 구역)
	
	// 시각화 상태값
	FVector PrevLocation = FVector::ZeroVector;			//이전 프레임 위치 (궤적 그릴 때 사용)
	float LastSpeed = 0.f;								// 이전 프레임 속도 (급감속 판별용)
	float LastDebugStringDistance = 0.f;				// 마지막으로 DrawDebugString 표시한 누적 거리
	FVector2D LastControlInput = FVector2D::ZeroVector; //마지막으로 받은 조향 / 스로틀(엑셀) 입력값
};
