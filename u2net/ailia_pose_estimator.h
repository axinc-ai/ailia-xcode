/*******************************************************************
*
*    DESCRIPTION:
*      AILIA 骨格検出・顔特徴点検出ライブラリ
*    AUTHOR:
*      AXELL Corporation
*    DATE:2020/06/22
*
*******************************************************************/

#if       !defined(INCLUDED_AILIA_POSE_ESTIMATOR)
#define            INCLUDED_AILIA_POSE_ESTIMATOR

/* コアライブラリ */

#include "ailia.h"
#include "ailia_format.h"

/* 呼び出し規約 */

#ifdef __cplusplus
extern "C" {
#endif

	/****************************************************************
	* 検出オブジェクトのインスタンス
	**/

	struct AILIAPoseEstimator;

	/****************************************************************
	* 物体情報
	**/

	#define AILIA_POSE_ESTIMATOR_ALGORITHM_ACCULUS_POSE   ( 0) // 骨格検出
	#define AILIA_POSE_ESTIMATOR_ALGORITHM_ACCULUS_FACE   ( 1) // 顔特徴点検出
	#define AILIA_POSE_ESTIMATOR_ALGORITHM_ACCULUS_UPPOSE ( 2) // 近接上半身姿勢検出
	#define AILIA_POSE_ESTIMATOR_ALGORITHM_ACCULUS_UPPOSE_FPGA ( 3) // 近接上半身2姿勢検出(FPGA向け)
	#define AILIA_POSE_ESTIMATOR_ALGORITHM_ACCULUS_HAND   ( 5) // 手姿勢検出
	#define AILIA_POSE_ESTIMATOR_ALGORITHM_OPEN_POSE      (10) // 骨格検出
	#define AILIA_POSE_ESTIMATOR_ALGORITHM_LW_HUMAN_POSE  (11) // 骨格検出

	/* 骨格検出 関節点 定義 */
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_NOSE				(0)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_EYE_LEFT			(1)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_EYE_RIGHT		(2)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_EAR_LEFT			(3)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_EAR_RIGHT		(4)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_SHOULDER_LEFT	(5)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_SHOULDER_RIGHT	(6)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_ELBOW_LEFT		(7)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_ELBOW_RIGHT		(8)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_WRIST_LEFT		(9)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_WRIST_RIGHT		(10)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_HIP_LEFT			(11)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_HIP_RIGHT		(12)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_KNEE_LEFT		(13)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_KNEE_RIGHT		(14)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_ANKLE_LEFT		(15)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_ANKLE_RIGHT		(16)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_SHOULDER_CENTER	(17)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_BODY_CENTER		(18)
	#define AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_CNT		(19)	// 個数

	/* 顔特徴点検出 定義 */
	#define AILIA_POSE_ESTIMATOR_FACE_KEYPOINT_CNT		(68)	// 個数

	/* 近接上半身姿勢検出 関節点 定義 */
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_NOSE				(0)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_EYE_LEFT			(1)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_EYE_RIGHT			(2)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_EAR_LEFT			(3)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_EAR_RIGHT			(4)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_SHOULDER_LEFT		(5)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_SHOULDER_RIGHT		(6)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_ELBOW_LEFT			(7)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_ELBOW_RIGHT		(8)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_WRIST_LEFT			(9)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_WRIST_RIGHT		(10)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_HIP_LEFT			(11)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_HIP_RIGHT			(12)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_SHOULDER_CENTER	(13)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_BODY_CENTER		(14)
	#define AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_CNT				(15)	// 個数

	/* 手姿勢検出 点 定義 */
	#define AILIA_POSE_ESTIMATOR_HAND_KEYPOINT_CNT		(21)	// 個数

	typedef struct _AILIAPoseEstimatorKeypoint {
		float x;						// 入力画像内 X座標  (0.0 , 1.0)
		float y;						// 入力画像内 Y座標  (0.0 , 1.0)
		float z_local;					// 骨格検出のみ有効。体中心を座標0とした時に推定されるローカルZ座標。単位(スケール)は x と同じです。
		float score;					// この点の検出信頼度。値が0.0Fの場合、この点は未検出のため使用できません。
		int interpolated;				// 通常は0です。この点が未検出で、他の点から補間可能な場合、x,yの値を補間し、interpolated=1となります。
	}AILIAPoseEstimatorKeypoint;

	#define AILIA_POSE_ESTIMATOR_OBJECT_POSE_VERSION (1)	// 構造体フォーマットバージョン
	typedef struct _AILIAPoseEstimatorObjectPose {
		AILIAPoseEstimatorKeypoint points[AILIA_POSE_ESTIMATOR_POSE_KEYPOINT_CNT];	// 検出した関節点。配列インデックスが関節番号に相当します。
		float total_score;			// このオブジェクトの検出信頼度
		int num_valid_points;		// points[]の中で正常に検出された関節点の個数
		int id;						// 時間方向に、このオブジェクトにユニークなIDです。1以上の正の値です。
		float angle[3];				// このオブジェクトのオイラー角 yaw, pitch, roll [単位radian]。現在yawのみ対応しています。角度が検出されない場合FLT_MAXが格納されます。
	}AILIAPoseEstimatorObjectPose;

	#define AILIA_POSE_ESTIMATOR_OBJECT_FACE_VERSION (1)	// 構造体フォーマットバージョン
	typedef struct _AILIAPoseEstimatorObjectFace {
		AILIAPoseEstimatorKeypoint points[AILIA_POSE_ESTIMATOR_FACE_KEYPOINT_CNT];	// 検出した顔特徴点。配列インデックスが顔特徴点番号に相当します。
		float total_score;			// このオブジェクトの検出信頼度
	}AILIAPoseEstimatorObjectFace;

	#define AILIA_POSE_ESTIMATOR_OBJECT_UPPOSE_VERSION (1)	// 構造体フォーマットバージョン
	typedef struct _AILIAPoseEstimatorObjectUpPose {
		AILIAPoseEstimatorKeypoint points[AILIA_POSE_ESTIMATOR_UPPOSE_KEYPOINT_CNT];	// 検出した関節点。配列インデックスが関節番号に相当します。
		float total_score;			// このオブジェクトの検出信頼度
		int num_valid_points;		// points[]の中で正常に検出された関節点の個数
		int id;						// 時間方向に、このオブジェクトにユニークなIDです。1以上の正の値です。
		float angle[3];				// このオブジェクトのオイラー角 yaw, pitch, roll [単位radian]。現在yawのみ対応しています。角度が検出されない場合FLT_MAXが格納されます。 
	}AILIAPoseEstimatorObjectUpPose;

	#define AILIA_POSE_ESTIMATOR_OBJECT_HAND_VERSION (1)	// 構造体フォーマットバージョン
	typedef struct _AILIAPoseEstimatorObjectHand {
		AILIAPoseEstimatorKeypoint points[AILIA_POSE_ESTIMATOR_HAND_KEYPOINT_CNT];	// 検出した関節点。配列インデックスが関節番号に相当します。
		float total_score;			// このオブジェクトの検出信頼度
	}AILIAPoseEstimatorObjectHand;

	/****************************************************************
	* 骨格検出・顔特徴点検出API
	**/

	/**
	*  検出オブジェクトを作成します。
	*    引数:
	*      pose_estimator - 検出オブジェクトポインタ
	*      net            - ネットワークオブジェクトポインタ
	*      algorithm      - AILIA_POSE_ESTIMATOR_ALGORITHM_*
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      caffemodelとprototxtを読み込んだAILIANetworkから検出オブジェクトを作成します。
	*/
	int AILIA_API ailiaCreatePoseEstimator(struct AILIAPoseEstimator **pose_estimator, struct AILIANetwork *net, unsigned int algorithm);

	/**
	*  検出オブジェクトを破棄します。
	*    引数:
	*      pose_estimator - 検出オブジェクトポインタ
	*/
	void AILIA_API ailiaDestroyPoseEstimator(struct AILIAPoseEstimator *pose_estimator);

	/**
	*  検出閾値を設定します。手姿勢検出（AILIA_POSE_ESTIMATOR_ALGORITHM_ACCULUS_HAND）でのみ有効です。
	*    引数:
	*      pose_estimator              - 検出オブジェクトポインタ
	*      threshold                   - 検出閾値 0.0以上1.0以下の値で、値が小さいほど検出しやすくなります。
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaPoseEstimatorSetThreshold(struct AILIAPoseEstimator *pose_estimator, float threshold);

	/**
	*  骨格検出・顔特徴点検出を行います。
	*    引数:
	*      pose_estimator              - 検出オブジェクトポインタ
	*      src                         - 画像データ(32bpp)
	*      src_stride                  - 1ラインのバイト数
	*      src_width                   - 画像幅
	*      src_height                  - 画像高さ
	*      src_format                  - AILIA_IMAGE_FORMAT_*
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaPoseEstimatorCompute(struct AILIAPoseEstimator *pose_estimator, const void *src, unsigned int src_stride, unsigned int src_width, unsigned int src_height, unsigned int src_format);

	/**
	*  検出結果の数を取得します。
	*    引数:
	*      pose_estimator  - 検出オブジェクトポインタ
	*      obj_count       - オブジェクト数  顔特徴点の場合は1または0となります。
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaPoseEstimatorGetObjectCount(struct AILIAPoseEstimator *pose_estimator, unsigned int *obj_count);

	/**
	*  骨格検出結果を取得します。
	*    引数:
	*      pose_estimator  - 検出オブジェクトポインタ
	*      obj             - オブジェクト情報
	*      obj_idx         - オブジェクトインデックス
	*      version         - AILIA_POSE_ESTIMATOR_OBJECT_POSE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaPoseEstimatorGetObjectPose(struct AILIAPoseEstimator *pose_estimator, AILIAPoseEstimatorObjectPose* obj, unsigned int obj_idx, unsigned int version);

	/**
	*  顔特徴点検出結果を取得します。
	*    引数:
	*      pose_estimator  - 検出オブジェクトポインタ
	*      obj             - オブジェクト情報
	*      obj_idx         - オブジェクトインデックス  必ず 0 を指定してください。
	*      version         - AILIA_POSE_ESTIMATOR_OBJECT_FACE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaPoseEstimatorGetObjectFace(struct AILIAPoseEstimator *pose_estimator, AILIAPoseEstimatorObjectFace* obj, unsigned int obj_idx, unsigned int version);

	/**
	*  UpPose 認識結果を取得します。
	*    引数:
	*      pose_estimator  - 検出オブジェクトポインタ
	*      obj             - オブジェクト情報
	*      obj_idx         - オブジェクトインデックス
	*      version         - AILIA_POSE_ESTIMATOR_OBJECT_POSE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaPoseEstimatorGetObjectUpPose(struct AILIAPoseEstimator *pose_estimator, AILIAPoseEstimatorObjectUpPose* obj, unsigned int obj_idx, unsigned int version);

	/**
	*  Hand 認識結果を取得します。
	*    引数:
	*      pose_estimator  - 検出オブジェクトポインタ
	*      obj             - オブジェクト情報
	*      obj_idx         - オブジェクトインデックス		必ず 0 を指定してください。
	*      version         - AILIA_POSE_ESTIMATOR_OBJECT_HAND_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaPoseEstimatorGetObjectHand(struct AILIAPoseEstimator *pose_estimator, AILIAPoseEstimatorObjectHand* obj, unsigned int obj_idx, unsigned int version);

#ifdef __cplusplus
}
#endif
#endif /* !defined(INCLUDED_AILIA_POSE_ESTIMATOR) */
