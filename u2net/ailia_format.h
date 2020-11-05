/*******************************************************************
*
*    DESCRIPTION:
*      AILIA フォーマット定義・変換
*    AUTHOR:
*      AXELL Corporation
*    DATE:2019/08/27
*
*******************************************************************/

#if       !defined(INCLUDED_AILIA_FORMAT)
#define            INCLUDED_AILIA_FORMAT

/* 呼び出し規約 */

#if defined(_WIN64) || defined(_M_X64) || defined(__amd64__) || defined(__x86_64__) || defined(__APPLE__) || defined(__ANDROID__) || defined(ANDROID) || defined(__linux__)
#define AILIA_API
#else
#define AILIA_API __stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/****************************************************************
	* 入力画像フォーマット
	**/

	#define AILIA_IMAGE_FORMAT_RGBA        (0x00)	//RGBA順
	#define AILIA_IMAGE_FORMAT_BGRA        (0x01)	//BGRA順

	#define AILIA_IMAGE_FORMAT_RGBA_B2T    (0x10)	//RGBA順(Bottom to Top)
	#define AILIA_IMAGE_FORMAT_BGRA_B2T    (0x11)	//BGRA順(Bottom to Top)

	/****************************************************************
	* ネットワーク画像フォーマット
	**/

	#define AILIA_NETWORK_IMAGE_FORMAT_BGR               (0)	//BGR順
	#define AILIA_NETWORK_IMAGE_FORMAT_RGB               (1)	//RGB順
	#define AILIA_NETWORK_IMAGE_FORMAT_GRAY              (2)	//GrayScale (1ch)
	#define AILIA_NETWORK_IMAGE_FORMAT_GRAY_EQUALIZE     (3)	//平坦化GrayScale (1ch)

	#define AILIA_NETWORK_IMAGE_CHANNEL_FIRST            (0)	//DCYX順
	#define AILIA_NETWORK_IMAGE_CHANNEL_LAST             (1)	//DYXC順

	#define AILIA_NETWORK_IMAGE_RANGE_UNSIGNED_INT8      (0)	//0 ～ 255
	#define AILIA_NETWORK_IMAGE_RANGE_SIGNED_INT8        (1)	//-128 ～ 127
	#define AILIA_NETWORK_IMAGE_RANGE_UNSIGNED_FP32      (2)	//0.0 ～ 1.0
	#define AILIA_NETWORK_IMAGE_RANGE_SIGNED_FP32        (3)	//-1.0 ～ 1.0


	/**
	*  画像のフォーマットを変換します。
	*    引数:
	*      dst                  - 変換後画像の格納先(numeric型、sizeof(float) * dst_width * dst_height * dst_channel [byte]以上のサイズを確保すること)
	*      dst_width            - 変換後画像の横幅
	*      dst_height           - 変換後画像の高さ
	*      dst_format           - 変換後画像の形式（AILIA_NETWORK_IMAGE_FORMAT_*）
	*      dst_channel          - 変換後画像のチャンネル順（AILIA_NETWORK_IMAGE_CHANNEL_*）
	*      dst_range            - 変換後画像のレンジ（AILIA_NETWORK_IMAGE_RANGE_*）
	*      src                  - 変換元画像の格納先(32bpp)
	*      src_stride           - 変換元画像のラインバイト数
	*      src_width            - 変換元画像の横幅
	*      src_height           - 変換元画像の高さ
	*      src_format           - 変換元画像の形式（AILIA_IMAGE_FORMAT_*）
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      画像フォーマットを変更します。dst_formatがAILIA_NETWORK_IMAGE_FORMAT_BGRもしくはAILIA_NETWORK_IMAGE_FORMAT_RGB
	*      の場合、チャンネル数は3, AILIA_NETWORK_IMAGE_FORMAT_GRAYの場合チャンネル数は1となります。
	*/

	int AILIA_API ailiaFormatConvert(void *dst, unsigned int dst_width, unsigned int dst_height, unsigned int dst_format, unsigned int dst_channel, unsigned int dst_range, const void *src, int src_stride, unsigned int src_width, unsigned int src_height, unsigned int src_format);

#ifdef __cplusplus
}
#endif
#endif /* !defined(INCLUDED_AILIA_FORMAT) */
