/*******************************************************************
*
*    DESCRIPTION:
*      AILIA 物体検出ライブラリ
*    AUTHOR:
*      AXELL Corporation
*    DATE:2020/02/20
*
*******************************************************************/

#if       !defined(INCLUDED_AILIA_DETECTOR)
#define            INCLUDED_AILIA_DETECTOR

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

	struct AILIADetector;

	/****************************************************************
	* 物体情報
	**/
	#define AILIA_DETECTOR_OBJECT_VERSION (1)

	typedef struct _AILIADetectorObject {
		unsigned int category;	// オブジェクトカテゴリ番号(0～category_count-1)
		float prob;		// 推定確率(0～1)
		float x;		// 左上X位置(1で画像幅)
		float y;		// 左上Y位置(1で画像高さ)
		float w;		// 幅(1で画像横幅、負数は取らない)
		float h;		// 高さ(1で画像高さ、負数は取らない)
	}AILIADetectorObject;

	#define AILIA_DETECTOR_ALGORITHM_YOLOV1 (0)	//YOLOV1
	#define AILIA_DETECTOR_ALGORITHM_YOLOV2 (1)	//YOLOV2
	#define AILIA_DETECTOR_ALGORITHM_YOLOV3 (2)	//YOLOV3
	#define AILIA_DETECTOR_ALGORITHM_SSD    (8)	//SSD(Single Shot multibox Detector)

	#define AILIA_DETECTOR_FLAG_NORMAL      (0)	//オプションなし

	/****************************************************************
	* 物体検出API
	**/

	/**
	*  検出オブジェクトを作成します。
	*    引数:
	*      detector       - 検出オブジェクトポインタ
	*      net            - ネットワークオブジェクトポインタ
	*      format         - ネットワークの画像フォーマット （AILIA_NETWORK_IMAGE_FORMAT_*）
	*      channel        - ネットワークの画像チャンネル (AILIA_NETWORK_IMAGE_CHANNEL_*)
	*      range          - ネットワークの画像レンジ （AILIA_NETWORK_IMAGE_RANGE_*）
	*      algorithm      - AILIA_DETECTOR_ALGORITHM_*
	*      caregory_count - 検出カテゴリ数(VOCの場合は20、COCOの場合は80、などを指定)
	*      flags          - AILIA_DETECTOR_FLAG_*
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaCreateDetector(struct AILIADetector **detector,struct AILIANetwork *net, unsigned int format, unsigned int channel, unsigned int range, unsigned int algorithm, unsigned int category_count, unsigned int flags);

	/**
	*  検出オブジェクトを破棄します。
	*    引数:
	*      detector - 検出オブジェクトポインタ
	*/
	void AILIA_API ailiaDestroyDetector(struct AILIADetector *detector);

	/**
	*  物体検出を行います。
	*    引数:
	*      detector                    - 検出オブジェクトポインタ
	*      src                         - 画像データ(32bpp)
	*      src_stride                  - 1ラインのバイト数
	*      src_width                   - 画像幅
	*      src_height                  - 画像高さ
	*      src_format                  - AILIA_IMAGE_FORMAT_*
	*      threshold                   - 検出しきい値(0.1f等)(小さいほど検出されやすくなり、検出数増加)
	*      iou                         - 重複除外しきい値(0.45f等)(小さいほど重複を許容せず検出数減少)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaDetectorCompute(struct AILIADetector *detector, const void *src, unsigned int src_stride, unsigned int src_width, unsigned int src_height, unsigned int src_format, float threshold, float iou);

	/**
	*  検出結果の数を取得します。
	*    引数:
	*      detector	  - 検出オブジェクトポインタ
	*      obj_count  - オブジェクト数
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaDetectorGetObjectCount(struct AILIADetector *detector, unsigned int *obj_count);

	/**
	*  検出結果を取得します。
	*    引数:
	*      detector	  - 検出オブジェクトポインタ
	*      obj        - オブジェクト情報
	*      obj_idx    - オブジェクトインデックス
	*      version    - AILIA_DETECTOR_OBJECT_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      ailiaPredictを一度も実行していない場合はAILIA_STATUS_INVALID_STATEが返ります。
	*      検出結果は推定確率順でソートされます。
	*/
	int AILIA_API ailiaDetectorGetObject(struct AILIADetector *detector, AILIADetectorObject* obj, unsigned int obj_idx, unsigned int version);

	/**
	*  YoloV2などのためにアンカーズ（anchorsまたはbiases）の情報を設定します。
	*    引数:
	*      detector	      - 検出オブジェクトポインタ
	*      anchors        - アンカーズの寸法（検出ボックスの形状、高さと幅）
	*      anchors_count  - アンカーズの数（anchorsの配列サイズの半分）
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      YoloV2などは学習時に決定された複数の検出ボックスを使用して物体検出を行います。このAPIで学習時に決定された検出ボックスの形状を設定することで、正しい推論を行います。
	*      anchorsには{x,y,x,y...}の形式で格納します。
	*      anchors_countが5の場合、anchorsは10次元の配列になります。
	*/
	int AILIA_API ailiaDetectorSetAnchors(struct AILIADetector *detector, float *anchors, unsigned int anchors_count);

	/**
	*  YoloV3でのモデルへの入力画像サイズを指定します。
	*    引数:
	*      detector	      - 検出オブジェクトポインタ
	*      input_width    - モデルの入力画像幅
	*      input_height   - モデルの入力画像高さ
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      YoloV3では単一のモデルが任意の入力解像度に対応します。(32 の倍数制限あり)
	*      計算量の削減等でモデルへの入力画像サイズを指定する場合この API を実行してください。
	*      ailiaCreateDetector() と ailiaDetectorCompute() の間に実行する必要があります。
	*      この API を実行しない場合、デフォルトの 416x416 を利用します。
	*      YOLOv3 以外で実行した場合、AILIA_STATUS_INVALID_STATE を返します。 
	*/
	int AILIA_API ailiaDetectorSetInputShape(struct AILIADetector *detector, unsigned int input_width, unsigned int input_height);

#ifdef __cplusplus
}
#endif
#endif /* !defined(INCLUDED_AILIA_DETECTOR) */
