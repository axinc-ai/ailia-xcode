/*******************************************************************
*
*    DESCRIPTION:
*      AILIA 物体識別ライブラリ
*    AUTHOR:
*      AXELL Corporation
*    DATE:2019/06/24
*
*******************************************************************/

#if       !defined(INCLUDED_AILIA_CLASSIFIER)
#define            INCLUDED_AILIA_CLASSIFIER

/* コアライブラリ */

#include "ailia.h"
#include "ailia_format.h"

/* 呼び出し規約 */

#ifdef __cplusplus
extern "C" {
#endif

	/****************************************************************
	* 識別オブジェクトのインスタンス
	**/

	struct AILIAClassifier;

	/****************************************************************
	* 識別情報
	**/

	#define AILIA_CLASSIFIER_CLASS_VERSION (1)

	typedef struct _AILIAClassifierClass {
		int category;	// 識別カテゴリ番号
		float prob;		// 推定確率(0～1)
	}AILIAClassifierClass;

	/**
	*  識別オブジェクトを作成します。
	*    引数:
	*      classifier - 識別オブジェクトポインタへのポインタ
	*      net        - ネットワークオブジェクトポインタ
	*      format     - ネットワークの画像フォーマット （AILIA_NETWORK_IMAGE_FORMAT_*）
	*      channel    - ネットワークの画像チャンネル (AILIA_NETWORK_IMAGE_CHANNEL_*)
	*      range      - ネットワークの画像レンジ （AILIA_NETWORK_IMAGE_RANGE_*）
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*     ネットワークオブジェクトを作成します。
	*     推論実行環境を自動にした場合はCPUモードになり、BLASが利用できる場合はBLASを利用します。
	*/

	int AILIA_API ailiaCreateClassifier(struct AILIAClassifier **classifier, struct AILIANetwork *net, unsigned int format, unsigned int channel, unsigned int range);

	/**
	*  識別オブジェクトを破棄します。
	*    引数:
	*      classifier - 識別オブジェクトポインタ
	*/
	void AILIA_API ailiaDestroyClassifier(struct AILIAClassifier *classifier);

	/**
	*  物体識別を行います。
	*    引数:
	*      classifier                  - 識別オブジェクトポインタ
	*      src                         - 画像データ(32bpp)
	*      src_stride                  - 1ラインのバイト数
	*      src_width                   - 画像幅
	*      src_height                  - 画像高さ
	*      src_format                  - AILIA_IMAGE_FORMAT_*
	*      max_class_count             - 識別結果の数の最大
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaClassifierCompute(struct AILIAClassifier *classifier,const void *src, unsigned int src_stride, unsigned int src_width, unsigned int src_height, unsigned int src_format, unsigned int max_class_count);

	/**
	*  識別結果の数を取得します。
	*    引数:
	*      classifier - 識別オブジェクトポインタ
	*      cls_count  - クラス数
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaClassifierGetClassCount(struct AILIAClassifier *classifier, unsigned int *cls_count);

	/**
	*  識別結果を取得します。
	*    引数:
	*      classifier - 識別オブジェクトポインタ
	*      cls        - クラス情報
	*      cls_idx    - クラスインデックス
	*      version    - AILIA_CLASSIFIER_CLASS_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      ailiaPredictを一度も実行していない場合はAILIA_STATUS_INVALID_STATEが返ります。
	*      識別結果は推定確率順でソートされます。
	*/
	int AILIA_API ailiaClassifierGetClass(struct AILIAClassifier *classifier, AILIAClassifierClass* obj, unsigned int cls_idx, unsigned int version);

#ifdef __cplusplus
}
#endif
#endif /* !defined(INCLUDED_AILIA_CLASSIFIER) */
