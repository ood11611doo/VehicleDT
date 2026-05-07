#include "DataLogger/AgentDataLoggerComponent.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "DrawDebugHelpers.h"

UAgentDataLoggerComponent::UAgentDataLoggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UAgentDataLoggerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// UTM 기준점 계산
	OriginUtmZone = GetUtmZone(OriginLongitude);
	LatLonToUtm(OriginLatitude, OriginLongitude,
				OriginUtmZone, OriginUtmEasting, OriginUtmNorthing);

	if (bEnableLogging)
		StartRecording();
}

void UAgentDataLoggerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopRecording();
	Super::EndPlay(EndPlayReason);
}

void UAgentDataLoggerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsRecording)
		return;

	ElapsedRecordingTime += DeltaTime;
	TimeSinceLastSave += DeltaTime;

	const float SaveInterval = 1.0f / FMath::Max(SaveFrequencyHz, 0.1f);
	if (TimeSinceLastSave >= SaveInterval)
	{
		DrawDebugTrail(); // 궤적 시각화
		AppendRow();      // CSV 저장
		TimeSinceLastSave -= SaveInterval;
	}
}

void UAgentDataLoggerComponent::StartRecording()
{
	if (bIsRecording)
		return;

	CreateCsvFile();
	bIsRecording = true;
	TimeSinceLastSave = 0.f;
	ElapsedRecordingTime = 0.f;
}

void UAgentDataLoggerComponent::StopRecording()
{
	bIsRecording = false;
}

void UAgentDataLoggerComponent::CreateCsvFile()
{
	// 저장 폴더 만들기
	const FString OutputDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Output"));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectoryTree(*OutputDir);

	// 파일 이름에 현재 시간 넣기
	const FDateTime Now = FDateTime::Now();
	const FString FileName = FString::Printf(
		TEXT("AgentData-%04d_%02d-%02d-%02d-%02d-%02d.csv"),
		Now.GetYear(), Now.GetMonth(), Now.GetDay(),
		Now.GetHour(), Now.GetMinute(), Now.GetSecond()
	);

	CSVFilePath = FPaths::Combine(OutputDir, FileName);

	// 헤더 쓰기
	const FString Header =
		TEXT("Timestamp,World_X,World_Y,World_Z,UTM_Easting,UTM_Northing,UTM_Zone,Velocity_kmh,Yaw,Acceleration,Steering,Throttle\n");

	FFileHelper::SaveStringToFile(Header, *CSVFilePath,
		FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	
	UE_LOG(LogTemp, Log, TEXT("[AgentDataLogger] Recording to: %s  (%.1f Hz)"), *CSVFilePath, SaveFrequencyHz);
}

void UAgentDataLoggerComponent::AppendRow()
{
	const AActor* Owner = GetOwner();
	if (!Owner) return;

	const FVector WorldLoc = Owner->GetActorLocation(); // cm
	const FRotator WorldRot = Owner->GetActorRotation();
	const FVector Velocity = Owner->GetVelocity();      // cm/s

	const float SpeedKmh = Velocity.Size() * 0.01f * 3.6f;
	const float Yaw = WorldRot.Yaw;

	// 가속도 계산 (이전 프레임 속도 변화)
	const float Acceleration = (SpeedKmh - LastSpeed) / FMath::Max(TimeSinceLastSave, 0.001f);
	LastSpeed = SpeedKmh;

	// UTM 변환
	double UtmEasting = 0.0, UtmNorthing = 0.0;
	WorldToUtm(WorldLoc, UtmEasting, UtmNorthing);

	const FString Row = FString::Printf(
		TEXT("%.3f,%.2f,%.2f,%.2f,%.4f,%.4f,%d,%.2f,%.4f,%.4f,%.4f,%.4f\n"),
		ElapsedRecordingTime,
		WorldLoc.X, WorldLoc.Y, WorldLoc.Z,
		UtmEasting, UtmNorthing, OriginUtmZone,
		SpeedKmh, Yaw,
		Acceleration,
		LastControlInput.X, // 조향
		LastControlInput.Y  // 스로틀
	);

	FFileHelper::SaveStringToFile(Row, *CSVFilePath,
		FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,
		&IFileManager::Get(),
		EFileWrite::FILEWRITE_Append
	);
}

void UAgentDataLoggerComponent::DrawDebugTrail()
{
    // DebugDrawEnabled가 꺼져있으면 실행 안 함
    if (!bDebugDrawEnabled) return;

    const AActor* Owner = GetOwner();
    if (!Owner) return;

    const FVector CurrentLocation = Owner->GetActorLocation();
    const float SpeedKmh = Owner->GetVelocity().Size() * 0.01f * 3.6f;
    const float Yaw = Owner->GetActorRotation().Yaw;

    // 속도 기반 색상 매핑 (느리면 파랑 → 빠르면 빨강)
    const float MaxSpeedKmh = 130.f;
    const float SpeedRatio = FMath::Clamp(SpeedKmh / MaxSpeedKmh, 0.f, 1.f);
    const FLinearColor TrailColor = FLinearColor::LerpUsingHSV(
        FLinearColor::Blue, FLinearColor::Red, SpeedRatio);

    // 궤적 선 그리기 (이전 위치 → 현재 위치)
    if (!PrevLocation.IsZero())
    {
        DrawDebugLine(
            GetWorld(),
            PrevLocation,           // 시작점
            CurrentLocation,        // 끝점
            TrailColor.ToFColor(true),
            true,                   // 영구 표시
            -1.f,                   // Duration (-1 = 영구)
            0,
            2.f                     // 선 두께
        );
    }

    // 급감속 지점 표시
    const float SpeedDelta = SpeedKmh - LastSpeed;
    if (FMath::Abs(SpeedDelta) > HardBrakeThreshold)
    {
        DrawDebugPoint(
            GetWorld(),
            CurrentLocation,
            15.f,           // 포인트 크기
            FColor::Yellow,
            true,           // 영구 표시
            -1.f
        );
    }

    // 일정 거리마다 속도/yaw 수치 표시
    const float DistanceMoved = FVector::Dist(PrevLocation, CurrentLocation);
    LastDebugStringDistance += DistanceMoved;

    if (LastDebugStringDistance >= DebugStringInterval)
    {
        const FString Info = FString::Printf(
            TEXT("%.1f km/h | Yaw: %.1f"), SpeedKmh, Yaw);

        DrawDebugString(
            GetWorld(),
            CurrentLocation + FVector(0, 0, 50.f), // 차량 위에 표시
            Info,
            nullptr,
            FColor::White,
            -1.f    // 영구 표시
        );
        LastDebugStringDistance = 0.f;
    }

    // 현재 위치를 다음 프레임의 이전 위치로 저장
    PrevLocation = CurrentLocation;
}

void UAgentDataLoggerComponent::HandleLidarScan(const FLidarScanResult& Scan)
{
	 // 받은 포인트 수 로그 출력
    UE_LOG(LogTemp, Log, TEXT("[DataLogger] Lidar points received: %d"), Scan.PointCount);
}

void UAgentDataLoggerComponent::HandleControlOutput(float Value)
{
	LastControlInput.X = Value; // 조향값 저장
}

void UAgentDataLoggerComponent::HandleCameraFrame()
{
	// 카메라 프레임 캡처 완료 신호만 받음 - 처리 없음
}

void UAgentDataLoggerComponent::WorldToUtm(const FVector& WorldLocation,
	double& OutEasting, double& OutNorthing) const
{
	const double OffsetEastM  =  WorldLocation.X * 0.01;
	const double OffsetNorthM = -WorldLocation.Y * 0.01;

	OutEasting  = OriginUtmEasting  + OffsetEastM;
	OutNorthing = OriginUtmNorthing + OffsetNorthM;
}

int32 UAgentDataLoggerComponent::GetUtmZone(double Longitude)
{
	return FMath::FloorToInt((Longitude + 180.0) / 6.0) + 1;
}

void UAgentDataLoggerComponent::LatLonToUtm(double Lat, double Lon, int32 Zone, double& OutEasting, double& OutNorthing)
{
	// WGS-84 타원체 상수
	constexpr double a  = 6378137.0;            // 장반축 (m)
	constexpr double f  = 1.0 / 298.257223563;  // 편평률
	constexpr double k0 = 0.9996;               // UTM 축척 계수

	const double e2 = 2.0 * f - f * f;          // 제1 이심률 제곱
	const double ep2 = e2 / (1.0 - e2);         // 제2 이심률 제곱

	const double LatRad = FMath::DegreesToRadians(Lat);
	const double CentralMeridian = (Zone - 1) * 6.0 - 180.0 + 3.0; // 중앙 경선
	const double DeltaLon = FMath::DegreesToRadians(Lon - CentralMeridian); // 중앙 경선과의 경도 차

	const double SinLat = FMath::Sin(LatRad);
	const double CosLat = FMath::Cos(LatRad);
	const double TanLat = FMath::Tan(LatRad);

	const double N = a / FMath::Sqrt(1.0 - e2 * SinLat * SinLat); // 곡률 반경
	const double T = TanLat * TanLat;
	const double C = ep2 * CosLat * CosLat;
	const double A = CosLat * DeltaLon;

	// Meridional arc (M)
	const double e4 = e2 * e2;
	const double e6 = e4 * e2;
	const double M = a * (
		(1.0 - e2 / 4.0 - 3.0 * e4 / 64.0  - 5.0 * e6 / 256.0) * LatRad
		- (3.0 * e2 / 8.0 + 3.0 * e4 / 32.0 + 45.0 * e6 / 1024.0) * FMath::Sin(2.0 * LatRad)
		+ (15.0 * e4 / 256.0 + 45.0 * e6 / 1024.0) * FMath::Sin(4.0 * LatRad)
		- (35.0 * e6 / 3072.0) * FMath::Sin(6.0 * LatRad));

	const double A2 = A * A;
	const double A4 = A2 * A2;
	const double A6 = A4 * A2;

	OutEasting = k0 * N * (
		A
		+ (1.0 - T + C) * A2 * A / 6.0
		+ (5.0 - 18.0 * T + T * T + 72.0 * C - 58.0 * ep2) * A4 * A / 120.0
	) + 500000.0;   // false easting 좌표 (오프셋)

	OutNorthing = k0 * (M + N * TanLat * (
		A2 / 2.0
		+ (5.0 - T + 9.0 * C + 4.0 * C * C) * A4 / 24.0
		+ (61.0 - 58.0 * T + T * T + 600.0 * C - 330.0 * ep2) * A6 / 720.0
	));

	// Southern 오프셋
	if (Lat < 0.0)
		OutNorthing += 10000000.0;
}
