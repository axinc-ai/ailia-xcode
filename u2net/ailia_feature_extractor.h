/*******************************************************************
*
*    DESCRIPTION:
*      AILIA 特徴抽出ライブラリ
*    AUTHOR:
*      AXELL Corporation
*    DATE:2019/06/24
*
*******************************************************************/

#if       !defined(INCLUDED_AILIA_FEATURE_EXTRACTOR)
#define            INCLUDED_AILIA_FEATURE_EXTRACTOR

/* コアライブラリ */

#include "ailia.h"
#include "ailia_format.h"

/* 呼び出し規約 */

#ifdef __cplusplus
extern "C" {
#endif

	/****************************************************************
	* 特徴抽出オブジェクトのインスタンス
	**/

	struct AILIAFeatureExtractor;

	/****************************************************************
	* 距離設定
	**/

	#define AILIA_FEATURE_EXTRACTOR_DISTANCE_L2NORM (0)	/* L2ノルム */

	/**
	*  特徴抽出オブジェクトを作成します。
	*    引数:
	*      fextractor - 特徴抽出オブジェクトポインタ
	*      net        - ネットワークオブジェクトポインタ
	*      format     - ネットワークの画像フォーマット （AILIA_NETWORK_IMAGE_FORMAT_*）
	*      channel    - ネットワークの画像チャンネル (AILIA_NETWORK_IMAGE_CHANNEL_*)
	*      range      - ネットワークの画像レンジ （AILIA_NETWORK_IMAGE_RANGE_*）
	*      layer_name - 特徴に対応したレイヤーの名称 (VGG16の場合はfc1, NULLで最終レイヤー)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaCreateFeatureExtractor(struct AILIAFeatureExtractor **fextractor, struct AILIANetwork *net, unsigned int format, unsigned int channel, unsigned int range, const char *layer_name);

	/**
	*  特徴抽出オブジェクトを破棄します。
	*    引数:
	*      fextractor - 特徴抽出オブジェクトポインタ
	*/
	void AILIA_API ailiaDestroyFeatureExtractor(struct AILIAFeatureExtractor *fextractor);

	/**
	*  特徴の抽出を行います。
	*    引数:
	*      fextractor                  - 特徴抽出オブジェクトポインタ
	*      dst                         - 特徴の格納先ポインタ（numeric型）
	*      dst_size                    - dstのサイズ(byte)
	*      src                         - 画像データ(32bpp)
	*      src_stride                  - 1ラインのバイト数
	*      src_width                   - 画像幅
	*      src_height                  - 画像高さ
	*      src_format                  - AILIA_IMAGE_FORMAT_*
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaFeatureExtractorCompute(struct AILIAFeatureExtractor *fextractor, void *dst, unsigned int dst_size, const void *src, unsigned int src_stride, unsigned int src_width, unsigned int src_height, unsigned int src_format);

	/**
	*  特徴間の距離を計算します。
	*    引数:
	*      fextractor                  - 特徴抽出オブジェクトポインタ
	*      distance                    - 特徴間距離
	*      distance_type               - 特徴間距離の種別
	*      feature1                    - 一方の特徴の格納先ポインタ（numeric型）
	*      feature1_size               - dstのサイズ(byte)
	*      feature2                    - 他方の特徴の格納先ポインタ（numeric型）
	*      feature2_size               - dstのサイズ(byte)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaFeatureExtractorMatch(struct AILIAFeatureExtractor *fextractor, float *distance, unsigned int distace_type, const void *feature1, unsigned int feature1_size, const void *feature2, unsigned int feature2_size);

#ifdef __cplusplus
}
#endif
#endif /* !defined(INCLUDED_AILIA_FEATURE_EXTRACTOR) */
