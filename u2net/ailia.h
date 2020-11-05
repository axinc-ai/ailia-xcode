/*******************************************************************
*
*    DESCRIPTION:
*      AILIA 推論ライブラリ
*    AUTHOR:
*      AXELL Corporation
*    DATE:2020/09/07
*
*******************************************************************/

#if       !defined(INCLUDED_AILIA)
#define            INCLUDED_AILIA

/* 呼び出し規約 */

#if defined(_WIN64) || defined(_M_X64) || defined(__amd64__) || defined(__x86_64__) || defined(__APPLE__) || defined(__ANDROID__) || defined(ANDROID) || defined(__linux__)
#define AILIA_API
#else
#define AILIA_API __stdcall
#endif
#include "ailia_call.h"
#ifdef __cplusplus
extern "C" {
#endif
	/****************************************************************
	* ライブラリ状態定義
	**/

	#define AILIA_STATUS_SUCCESS							(   0)  /* 成功 */
	#define AILIA_STATUS_INVALID_ARGUMENT					(  -1)  /* 引数が不正 */
	#define AILIA_STATUS_ERROR_FILE_API						(  -2)  /* ファイルアクセスに失敗した */
	#define AILIA_STATUS_INVALID_VERSION					(  -3)  /* 構造体バージョンが不正 */
	#define AILIA_STATUS_BROKEN								(  -4)  /* 壊れたファイルが渡された */
	#define AILIA_STATUS_MEMORY_INSUFFICIENT				(  -5)  /* メモリが不足している */
	#define AILIA_STATUS_THREAD_ERROR						(  -6)  /* スレッドの作成に失敗した */
	#define AILIA_STATUS_INVALID_STATE						(  -7)  /* デコーダの内部状態が不正 */
	#define AILIA_STATUS_UNSUPPORT_NET						(  -9)  /* 非対応のネットワーク */
	#define AILIA_STATUS_INVALID_LAYER						( -10)  /* レイヤーの重みやパラメータ、入出力形状が不正 */
	#define AILIA_STATUS_INVALID_PARAMINFO					( -11)  /* パラメータファイルの内容が不正 */
	#define AILIA_STATUS_NOT_FOUND							( -12)  /* 指定した要素が見つからなかった */
	#define AILIA_STATUS_GPU_UNSUPPORT_LAYER				( -13)  /* GPUで未対応のレイヤーパラメータが与えられた */
	#define AILIA_STATUS_GPU_ERROR							( -14)  /* GPU上での処理中にエラー */
	#define AILIA_STATUS_UNIMPLEMENTED						( -15)  /* 未実装 */
	#define AILIA_STATUS_PERMISSION_DENIED					( -16)  /* 許可されていない操作 */
	#define AILIA_STATUS_EXPIRED                            ( -17)  /* モデルの有効期限切れ */
	#define AILIA_STATUS_UNSETTLED_SHAPE 					( -18)  /* 形状が未確定 */
	#define AILIA_STATUS_DATA_REMOVED 						( -19)  /* 最適化などにより削除された */
	#define AILIA_STATUS_LICENSE_NOT_FOUND                  ( -20)  /* 有効なライセンスが見つからない */
	#define AILIA_STATUS_LICENSE_BROKEN                     ( -21)  /* ライセンスが壊れている */
	#define AILIA_STATUS_LICENSE_EXPIRED                    ( -22)  /* ライセンスの有効期限切れ */
	#define AILIA_STATUS_NDIMENSION_SHAPE					( -23)  /* 形状が5次元以上であることを示す */
	#define AILIA_STATUS_OTHER_ERROR						(-128)  /* 不明なエラー */

	/****************************************************************
	* ネットワークオブジェクトのインスタンス
	**/

	struct AILIANetwork;

	/****************************************************************
	* 形状情報
	**/

	#define AILIA_SHAPE_VERSION (1)

	typedef struct _AILIAShape {
		unsigned int x;			// X軸のサイズ
		unsigned int y;			// Y軸のサイズ
		unsigned int z;			// Z軸のサイズ
		unsigned int w;			// W軸のサイズ 
		unsigned int dim;		// 次元情報
	}AILIAShape;

	/****************************************************************
	* スレッド数
	**/

	#define  AILIA_MULTITHREAD_AUTO (0)

	/****************************************************************
	* 推論実行環境自動設定
	**/

	#define  AILIA_ENVIRONMENT_ID_AUTO		(-1)

	/****************************************************************
	* 推論API
	**/

	/**
	*  ネットワークオブジェクトを作成します。
	*    引数:
	*      net        - ネットワークオブジェクトポインタへのポインタ
	*      env_id     - 計算に利用する推論実行環境のID(ailiaGetEnvironmentで取得) AILIA_ENVIRONMENT_ID_AUTOにした場合は自動で選択する
	*      num_thread - スレッド数の上限(AILIA_MULTITHREAD_AUTOにした場合は自動で設定)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*     ネットワークオブジェクトを作成します。
	*     推論実行環境を自動にした場合はCPUモードになり、BLASが利用できる場合はBLASを利用します。
	*     なお、BLASを利用する場合num_threadは無視される場合があります。
	*/
	int AILIA_API ailiaCreate(struct AILIANetwork **net, int env_id, int num_thread);

	/**
	*  ネットワークオブジェクトを初期化します。(ファイルから読み込み)
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      path      - prototxtファイルのパス名(MBSC or UTF16)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*      ファイルから読み込み、ネットワークオブジェクトを初期化します。
	*/
	int AILIA_API ailiaOpenStreamFileA(struct AILIANetwork *net, const char    *path);
	int AILIA_API ailiaOpenStreamFileW(struct AILIANetwork *net, const wchar_t *path);

	/**
	*  ネットワークオブジェクトを初期化します。(ユーザ定義ファイルアクセスコールバック)
	*    引数:
	*      net			 - ネットワークオブジェクトポインタ
	*      fopen_args	 - AILIA_USER_API_FOPENに通知される引数ポインタ
	*      callback		 - ユーザ定義ファイルアクセスコールバック関数構造体
	*      version		 - ファイルアクセスコールバック関数構造体のバージョン(AILIA_FILE_CALLBACK_VERSION)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*      ファイルから読み込み、ネットワークオブジェクトを初期化します。
	*/
	int AILIA_API ailiaOpenStreamEx(struct AILIANetwork *net, const void *fopen_args, ailiaFileCallback callback, int version);

	/**
	*  ネットワークオブジェクトを初期化します。(メモリから読み込み)
	*    引数:
	*      net			 - ネットワークオブジェクトポインタ
	*      buf　　　	 - prototxtファイルのデータへのポインタ
	*      buf_size		 - prototxtファイルのデータサイズ
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*      メモリから読み込み、ネットワークオブジェクトを初期化します。
	*/
	int AILIA_API ailiaOpenStreamMem(struct AILIANetwork *net, const void *buf, unsigned int buf_size);

	/**
	*  ネットワークオブジェクトに重み係数を読み込みます。(ファイルから読み込み)
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      path      - protobufファイルのパス名(MBSC or UTF16)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*      ファイルからネットワークオブジェクトに重み係数を読み込みます。
	*/
	int AILIA_API ailiaOpenWeightFileA(struct AILIANetwork *net, const char    *path);
	int AILIA_API ailiaOpenWeightFileW(struct AILIANetwork *net, const wchar_t *path);

	/**
	*  ネットワークオブジェクトに重み係数を読み込みます。(ユーザ定義ファイルアクセスコールバック)
	*    引数:
	*      net			 - ネットワークオブジェクトポインタ
	*      fopen_args	 - AILIA_USER_API_FOPENに通知される引数ポインタ
	*      callback		 - ユーザ定義ファイルアクセスコールバック関数構造体
	*      version		 - ファイルアクセスコールバック関数構造体のバージョン(AILIA_FILE_CALLBACK_VERSION)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*      ファイルからネットワークオブジェクトに重み係数を読み込みます。
	*/
	int AILIA_API ailiaOpenWeightEx(struct AILIANetwork *net, const void *fopen_args, ailiaFileCallback callback, int version);

	/**
	*  ネットワークオブジェクトに重み係数を読み込みます。(メモリから読み込み)
	*    引数:
	*      net		     - ネットワークオブジェクトポインタ
	*      buf　　　	 - protobufファイルのデータへのポインタ
	*      buf_size		 - protobufファイルのデータサイズ
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*      メモリからネットワークオブジェクトに重み係数を読み込みます。
	*/
	int AILIA_API ailiaOpenWeightMem(struct AILIANetwork *net, const void *buf, unsigned int buf_size);

	/**
	*  ネットワークオブジェクトを破棄します。
	*    引数:
	*      net - ネットワークオブジェクトポインタ
	*/
	void AILIA_API ailiaDestroy(struct AILIANetwork *net);

	/**
	*  推論時の入力データの形状を変更します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      shape     - 入力データの形状情報
	*      version   - AILIA_SHAPE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*      prototxtで定義されている入力形状を変更します。
	*      prototxtに記述されているランクと同じにする必要があります。
	*      なお、重み係数の形状が入力形状に依存しているなどによりエラーが返る場合があります。
	*      prototxtで定義されているランクが5次元以上の場合はailiaSetInputShapeNDを利用してください。
	*/
	int AILIA_API ailiaSetInputShape(struct AILIANetwork *net, const AILIAShape* shape, unsigned int version);

	/**
	*  推論時の入力データの形状を変更します。
	*    引数:
	*      net	     - ネットワークオブジェクトポインタ
	*      shape     - 入力データの各次元の大きさの配列(dim-1, dim-2, ... ,1, 0)
	*      dim       - shapeの次元
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*   解説:
	*      prototxtで定義されている入力形状を変更します。
	*      prototxtに記述されているランクと同じにする必要があります。
	*      なお、重み係数の形状が入力形状に依存しているなどによりエラーが返る場合があります。
	*/
	int AILIA_API ailiaSetInputShapeND(struct AILIANetwork* net, const unsigned int* shape, unsigned int dim);

	/**
	*  推論時の入力データの形状を取得します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      shape     - 入力データの形状情報
	*      version   - AILIA_SHAPE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、5次元以上の場合AILIA_STATUS_NDIMENSION_SHAPE、
	*      形状の一部が未確定の場合AILIA_STATUS_UNSETTLED_SHAPE、それ以外のエラーの場合はエラーコードを返す。
	*
	*      形状が5次元以上の場合はailiaGetInputDim、ailiaGetInputShapeNDを利用してください。
	*      形状の一部が未確定の場合、該当する次元の値は0となり、それ以外の次元の値は有効な値が格納されます。
	*/
	int AILIA_API ailiaGetInputShape(struct AILIANetwork *net, AILIAShape* shape, unsigned int version);

	/**
	*  推論時の入力データの次元を取得します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      dim       - 入力データの次元の格納先
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、それ以外のエラーの場合はエラーコードを返す。
	*/
	int AILIA_API ailiaGetInputDim(struct AILIANetwork* net, unsigned int* dim);

	/**
	*  推論時の入力データの形状を取得します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      shape     - 入力データの各次元の大きさの格納先配列(dim-1, dim-2, ... ,1, 0順で格納)
	*      dim       - shapeの次元
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、形状の一部が未確定の場合AILIA_STATUS_UNSETTLED_SHAPE、
	*      それ以外のエラーの場合はエラーコードを返す。
	*
	*      形状の一部が未確定の場合、該当する次元の値は0となり、それ以外の次元の値は有効な値が格納されます。
	*/
	int AILIA_API ailiaGetInputShapeND(struct AILIANetwork* net, unsigned int* shape, unsigned int dim);

	/**
	*  推論時の出力データの形状を取得します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      shape     - 出力データの形状情報
	*      version   - AILIA_SHAPE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、5次元以上の場合AILIA_STATUS_NDIMENSION_SHAPE、
	*      それ以外のエラーの場合エラーコードを返す。
	*      形状が5次元以上の場合はailiaGetOutputDim、ailiaGetOutputShapeNDを利用してください。
	*/
	int AILIA_API ailiaGetOutputShape(struct AILIANetwork *net, AILIAShape* shape, unsigned int version);

	/**
	*  推論時の出力データの次元を取得します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      dim       - 出力データの次元の格納先
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、それ以外のエラーの場合はエラーコードを返す。
	*/
	int AILIA_API ailiaGetOutputDim(struct AILIANetwork* net, unsigned int* dim);

	/**
	*  推論時の出力データの形状を取得します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      shape     - 出力データの各次元の大きさの格納先配列(dim-1, dim-2, ... ,1, 0順で格納)
	*      dim       - shapeの次元
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、それ以外のエラーの場合はエラーコードを返す。
	*/
	int AILIA_API ailiaGetOutputShapeND(struct AILIANetwork* net, unsigned int* shape, unsigned int dim);


	/**
	*  推論を行い推論結果を取得します。
	*    引数:
	*      net                         - ネットワークオブジェクトポインタ
	*      dest                        - 推論結果の書き出し先バッファにX,Y,Z,Wの順でnumeric型で格納  サイズはネットファイルのoutputSizeとなる
	*      dest_size                   - 推論結果の書き出し先バッファのbyte数
	*      src                         - 推論データ X,Y,Z,Wの順でnumeric型で格納 サイズはネットファイルのinputSizeとなる
	*      src_size                    - 推論データサイズ
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaPredict(struct AILIANetwork *net, void * dest, unsigned int dest_size, const void *src, unsigned int src_size);

	/****************************************************************
	* 状態取得API
	**/

	/**
	*  推論時の内部データ(Blob)の数を取得します。
	*    引数:
	*      net		  - ネットワークオブジェクトポインタ
	*      blob_count - blobの数の格納先
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaGetBlobCount(struct AILIANetwork *net, unsigned int *blob_count);

	/**
	*  推論時の内部データ(Blob)の形状を取得します。
	*    引数:
	*      net		  - ネットワークオブジェクトポインタ
	*      shape      - データの形状情報の格納先
	*      blob_idx   - blobのインデックス (0～ailiaGetBlobCount()-1)
	*      version    - AILIA_SHAPE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、5次元以上の場合AILIA_STATUS_NDIMENSION_SHAPE、
	*      それ以外のエラーの場合はエラーコードを返す。
	* 
	*      形状が5次元以上の場合はailiaGetBlobDim、ailiaGetBlobShapeNDを利用してください。
	*/
	int AILIA_API ailiaGetBlobShape(struct AILIANetwork *net, AILIAShape* shape, unsigned int blob_idx, unsigned int version);

	/**
	*  推論時の内部データ(Blob)の次元を取得します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      dim       - blobの次元の格納先
	*      blob_idx  - blobのインデックス (0～ailiaGetBlobCount()-1)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、それ以外のエラーの場合はエラーコードを返す。
	*/
	int AILIA_API ailiaGetBlobDim(struct AILIANetwork* net, unsigned int* dim, unsigned int blob_idx);

	/**
	*  推論時の内部データ(Blob)の形状を取得します。
	*    引数:
	*      net		 - ネットワークオブジェクトポインタ
	*      shape     - blobの各次元の大きさの格納先配列(dim-1, dim-2, ... ,1, 0順で格納)
	*      dim       - shapeの次元
	*      blob_idx  - blobのインデックス (0～ailiaGetBlobCount()-1)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、それ以外のエラーの場合はエラーコードを返す。
	*/
	int AILIA_API ailiaGetBlobShapeND(struct AILIANetwork* net, unsigned int* shape, unsigned int dim, unsigned int blob_idx);

	/**
	*  推論時の内部データ(Blob)を取得します。
	*    引数:
	*      net		  - ネットワークオブジェクトポインタ
	*      dest       - 推論結果の書き出し先バッファにX,Y,Z,Wの順でnumeric型で格納
	*      dest_size  - 推論結果の書き出し先バッファのbyte数
	*      blob_idx   - blobのインデックス (0～ailiaGetBlobCount()-1)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      ailiaPredictを一度も実行していない場合はAILIA_STATUS_INVALID_STATEが返ります。
	*/
	int AILIA_API ailiaGetBlobData(struct AILIANetwork *net, void* dest, unsigned int dest_size, unsigned int blob_idx);

	/**
	*  推論時の内部データ(Blob)のインデックスを名前で探し取得します。
	*    引数:
	*      net		  - ネットワークオブジェクトポインタ
	*      blob_idx   - blobのインデックス (0～ailiaGetBlobCount()-1)
	*      name       - 検索するBlob名
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaFindBlobIndexByName(struct AILIANetwork *net, unsigned int* blob_idx, const char* name);

	/**
	*  内部データ(Blob)の名前の出力に必要なバッファのサイズを取得します。
	*    引数:
	*      net		        - ネットワークオブジェクトポインタ
	*      blob_idx         - blobのインデックス (0～ailiaGetBlobCount()-1)
	*      buffer_size      - Blob名の出力に必要なバッファのサイズ(終端null文字分を含む)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaGetBlobNameLengthByIndex(struct AILIANetwork *net, const unsigned int blob_idx, unsigned int* buffer_size);

	/**
	*  推論時の内部データ(Blob)の名前をインデックスで探し取得します。
	*    引数:
	*      net		    - ネットワークオブジェクトポインタ
	*      buffer       - Blob名の出力先バッファ
	*      buffer_size  - バッファのサイズ(終端null文字分を含む)
	*      blob_idx     - 検索するblobのインデックス
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaFindBlobNameByIndex(struct AILIANetwork *net, char* buffer, const unsigned int buffer_size, const unsigned int blob_idx);

	/**
	*  ネットワークSummary用に必要なバッファのサイズを取得します。
	*    引数:
	*      net		    - ネットワークオブジェクトポインタ
	*      buffer_size  - バッファのサイズの格納先(終端null文字分を含む)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaGetSummaryLength(struct AILIANetwork * net, unsigned int * buffer_size);

	/**
	*  各Blobの名前と形状を表示します。
	*    引数:
	*      net		    - ネットワークオブジェクトポインタ
	*      buffer       - Summaryの出力先
	*      buffer_size  - 出力バッファのサイズ(終端null文字分を含む)
	*                     ailiaGetSummaryLengthで取得した値を設定してください。
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/

	int AILIA_API ailiaSummary(struct AILIANetwork * net, char* const buffer, const unsigned int buffer_size);

	/****************************************************************
	* 複数入力指定・推論API
	**/

	/**
	*  入力データ(Blob)の数を取得します。
	*    引数:
	*      net		  - ネットワークオブジェクトポインタ
	*      input_blob_count - 入力blobの数の格納先
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaGetInputBlobCount(struct AILIANetwork *net, unsigned int *input_blob_count);

	/**
	*  入力データ(Blob)のインデックスを取得します
	*    引数:
	*      net		  - ネットワークオブジェクトポインタ
	*      blob_idx - blobのインデックス(0～ailiaGetBlobCount()-1)
	*      input_blob_idx - 入力blob内でのインデックス(0～ailiaGetInputBlobCount()-1)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaGetBlobIndexByInputIndex(struct AILIANetwork *net, unsigned int *blob_idx, unsigned int input_blob_idx);

	/**
	*  指定したBlobに入力データを与えます。
	*    引数:
	*      net		    - ネットワークオブジェクトポインタ
	*      src          - 推論データ X,Y,Z,Wの順でnumeric型で格納
	*      src_size     - 推論データサイズ
	*      blob_idx     - 入力するblobのインデックス
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      複数入力があるネットワークなどで入力を指定する場合に用います。
	*      blob_idxで入力レイヤーのblob以外のものを指定した場合、AILIA_STATUS_INVALID_ARGUMENTが返ります。
	*/
	int AILIA_API ailiaSetInputBlobData(struct AILIANetwork *net, const void* src, unsigned int src_size, unsigned int blob_idx);

	/**
	*  指定したBlobの形状を変更します。
	*    引数:
	*      net      - ネットワークオブジェクトポインタ
	*      shape    - 入力データの形状情報
	*      blob_idx - 変更するblobのインデックス
	*      version  - AILIA_SHAPE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      複数入力があるネットワークなどで入力形状を変更する場合に用います。
	*      blob_idxは入力レイヤーのblob以外のものを指定した場合AILIA_STATUS_INVALID_ARGUMENTが返ります。
	*      その他の注意点はailiaSetInputShapeの解説を参照してください。
	*      入力形状のランクが5次元以上の場合はailiaSetInputBlobShapeNDを利用してください。
	*/
	int AILIA_API ailiaSetInputBlobShape(struct AILIANetwork *net, const AILIAShape* shape, unsigned int blob_idx, unsigned int version);


	/**
	*  指定したBlobの形状を変更します。
	*    引数:
	*      net      - ネットワークオブジェクトポインタ
	*      shape    - 入力データの各次元の大きさの配列(dim-1, dim-2, ... ,1, 0)
	*      dim      - shapeの次元
	*      blob_idx - 変更するblobのインデックス
	*      version  - AILIA_SHAPE_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      複数入力があるネットワークなどで入力形状を変更する場合に用います。
	*      blob_idxは入力レイヤーのblob以外のものを指定した場合AILIA_STATUS_INVALID_ARGUMENTが返ります。
	*      その他の注意点はailiaSetInputShapeNDの解説を参照してください。
	*/
	int AILIA_API ailiaSetInputBlobShapeND(struct AILIANetwork* net, const unsigned* shape, const unsigned dim, unsigned int blob_idx);

	/**
	*  事前に指定した入力データで推論を行います。
	*    引数:
	*      net		    - ネットワークオブジェクトポインタ
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      ailiaSetInputBlobDataを用いて入力を与えた場合などに用います。
	*      推論結果はailiaGetBlobDataで取得してください。
	*/
	int AILIA_API ailiaUpdate(struct AILIANetwork *net);

	/**
	*  出力データ(Blob)の数を取得します。
	*    引数:
	*      net		  - ネットワークオブジェクトポインタ
	*      output_blob_count - 出力blobの数の格納先
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaGetOutputBlobCount(struct AILIANetwork *net, unsigned int *output_blob_count);

	/**
	*  出力データ(Blob)のインデックスを取得します
	*    引数:
	*      net		  - ネットワークオブジェクトポインタ
	*      blob_idx - blobのインデックス(0～ailiaGetBlobCount()-1)
	*      output_blob_idx - 出力blob内でのインデックス(0～ailiaGetOutputBlobCount()-1)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaGetBlobIndexByOutputIndex(struct AILIANetwork *net, unsigned int *blob_idx, unsigned int output_blob_idx);

	/****************************************************************
	* 推論実行環境取得・指定API
	**/

	#define  AILIA_ENVIRONMENT_VERSION      (2)

	#define  AILIA_ENVIRONMENT_TYPE_CPU	    (0)
	#define  AILIA_ENVIRONMENT_TYPE_BLAS    (1)
	#define  AILIA_ENVIRONMENT_TYPE_GPU     (2)

	#define AILIA_ENVIRONMENT_BACKEND_NONE			(0)
	#define AILIA_ENVIRONMENT_BACKEND_AMP			(1)
	#define AILIA_ENVIRONMENT_BACKEND_CUDA			(2)
	#define AILIA_ENVIRONMENT_BACKEND_MPS			(3)
	#define AILIA_ENVIRONMENT_BACKEND_RENDERSCRIPT	(4)
	#define AILIA_ENVIRONMENT_BACKEND_OPENCL		(5)
	#define AILIA_ENVIRONMENT_BACKEND_VULKAN		(6)

	#define AILIA_ENVIRONMENT_PROPERTY_NORMAL			(0)
	#define AILIA_ENVIRONMENT_PROPERTY_LOWPOWER			(1)	// 省電力なGPU(内蔵GPUなど)を用いることを示す(MPS用)
	#define AILIA_ENVIRONMENT_PROPERTY_FP16				(2)	// FP16で動作することを示す

	typedef struct _AILIAEnvironment {
		int id;				// 環境を識別するID(ailiaCreateの引数に与える)
		int type;			// 環境の種別(AILIA_ENVIRONMENT_TYPE_CPU or BLAS or GPU)
		const char* name;	// デバイス名(シングルトンで保持されており開放不要)(ASCII)
		int backend;		// 環境のバックエンド (AILIA_ENVIRONMENT_BACKEND_*)
		int props;			// 環境の特性などを示す(AILIA_ENVIRONMENT_PROPERTY_*)
	}AILIAEnvironment;

	/**
	*  一時キャッシュディレクトリを指定します
	*    引数:
	*      cache_dir    - 一時キャッシュディレクトリ
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      指定したキャッシュディレクトリは推論実行環境毎に最適化したマシンコードを生成して保存するためにシステムが利用します。
	*      ailia の実行開始時に一度だけ呼び出してください。二回目以降の呼び出しに対しては無視して成功を返します。
	*      複数スレッドから呼び出された場合も内部で排他制御しているので特に問題は発生しません。
	*      android 環境での RenderScript など、この API を呼ぶまで利用できないものがあります。
	*      cache_dirにはContext.getCacheDir()で取得したファイルパスを指定してください。
	*      RenderScriptのPermissionの制約で外部ストレージのパスを与えることはできません。
	*/
	int AILIA_API ailiaSetTemporaryCachePathA(const char    * cache_dir);
	int AILIA_API ailiaSetTemporaryCachePathW(const wchar_t * cache_dir);

	/**
	*  利用可能な計算環境(CPU, GPU)の数を取得します
	*    引数:
	*      env_count     - 計算環境情報の数の格納先
	*    返値:
	*      利用可能な計算環境の件数
	*/
	int AILIA_API ailiaGetEnvironmentCount(unsigned int * env_count);

	/**
	*  計算環境の一覧を取得します
	*    引数:
	*      env          - 計算環境情報の格納先(AILIANetworkインスタンスを破棄するまで有効)
	*      env_idx    - 計算環境情報のインデックス(0～ailiaGetEnvironmentCount()-1)
	*      version      - AILIA_ENVIRONMENT_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/

	int AILIA_API ailiaGetEnvironment(AILIAEnvironment** env, unsigned int env_idx, unsigned int version);

	/**
	*  選択された計算環境を取得します
	*    引数:
	*      net        - ネットワークオブジェクトポインタ
	*      env          - 計算環境情報の格納先(AILIANetworkインスタンスを破棄するまで有効)
	*      version      - AILIA_ENVIRONMENT_VERSION
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*/
	int AILIA_API ailiaGetSelectedEnvironment(struct AILIANetwork *net, AILIAEnvironment** env, unsigned int version);

	/****************************************************************
	* メモリモードAPI
	**/

	#define AILIA_MEMORY_NO_OPTIMIZATION                        (0) // 中間バッファーの開放は行わない
	#define AILIA_MEMORY_REDUCE_CONSTANT                        (1) // 重みなどの定数となる中間バッファーを開放する
	#define AILIA_MEMORY_REDUCE_CONSTANT_WITH_INPUT_INITIALIZER (2) // 入力指定のinitializerを変更不可にし、重みなどの定数となる中間バッファーを開放する
	#define AILIA_MEMORY_REDUCE_INTERSTAGE                      (4) // 推論時の中間バッファーを開放する
	#define AILIA_MEMORY_REUSE_INTERSTAGE                       (8) // 中間バッファーを共有して推論する。AILIA_MEMORY_REDUCE_INTERSTAGEと併用した場合、共有可能な中間バッファーは開放しない。

	#define AILIA_MEMORY_OPTIMAIZE_DEFAULT (AILIA_MEMORY_REDUCE_CONSTANT)

	/**
	*  推論時のメモリの使用方針を設定します
	*    引数:
	*      net        - ネットワークオブジェクトポインタ
	*      mode       - メモリモード(論理和で複数指定可) AILIA_MEMORY_XXX (デフォルト:AILIA_MEMORY_REDUCE_CONSTANT)
	*    返値:
	*      成功した場合はAILIA_STATUS_SUCCESS、そうでなければエラーコードを返す。
	*    解説:
	*      メモリの使用方針を変更します。AILIA_MEMORY_NO_OPTIMIZATION以外を指定した場合は、
	*      推論時に確保する中間バッファーを開放するため、推論時のメモリ使用量を削減することができます。
	*      ailiaCreateの直後に指定する必要があります。ailiaOpenを呼び出した後は変更することができません。
	*      なお、中間バッファーを開放するように指定した場合、該当するBlobに対し、ailiaGetBlobDataを呼び出すと
	*      AILIA_STATUS_DATA_REMOVEDエラーが返ります。
	*/
	int AILIA_API ailiaSetMemoryMode(struct AILIANetwork* net, unsigned int mode);

	/****************************************************************
	* 情報取得API
	**/

	/**
	* エラーの詳細を返します
	* 返値:
	*   エラー詳細
	* 解説:
	*   返値は解放する必要はありません。
	*   文字列の有効期間は次にailiaのAPIを呼ぶまでです。
	*/
	const char * AILIA_API ailiaGetErrorDetail(struct AILIANetwork *net);

	/**
	* ライブラリバージョンを取得します
	* 返値:
	*   バージョン番号
	* 解説:
	*   返値は解放する必要はありません。
	*/
	const char* AILIA_API ailiaGetVersion(void);

#ifdef UNICODE
#define ailiaOpenStreamFile					ailiaOpenStreamFileW
#define ailiaOpenWeightFile					ailiaOpenWeightFileW
#define ailiaSetTemporaryCachePath          ailiaSetTemporaryCachePathW
#else
#define ailiaOpenStreamFile					ailiaOpenStreamFileA
#define ailiaOpenWeightFile					ailiaOpenWeightFileA
#define ailiaSetTemporaryCachePath          ailiaSetTemporaryCachePathA
#endif

#ifdef __cplusplus
}
#endif
#endif /* !defined(INCLUDED_AILIA) */
