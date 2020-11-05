/*******************************************************************
*
*    DESCRIPTION:
*      AILIA ユーザ定義コールバック
*    AUTHOR:
*      AXELL Corporation
*    DATE:2020/06/01
*
*******************************************************************/

#if       !defined(INCLUDED_AILIA_CALL)
#define            INCLUDED_AILIA_CALL

/* 呼び出し規約 */

#if defined(_WIN32) && !defined(_WIN64)
#define AILIA_USER_API __stdcall
#else
#define AILIA_USER_API
#endif

/* ユーザ定義コールバックの返値 */
#define AILIA_USER_API_SUCCESS ( 0)  /* 成功 */
#define AILIA_USER_API_FAILED  (-1)  /* 失敗 */

/**
* ファイルアクセスコールバック関数は、1ネットワークオブジェクトにつき、
* 1スレッドから呼び出されます。
* 複数のオブジェクトに対して、共通のコールバック関数を与える場合は、
* コールバック関数はスレッドセーフである必要があります。
* また、コールバック関数からは例外を投げずに、
* AILIA_USER_API_FAILEDでエラーを通知してください。
*/


#ifdef __cplusplus
extern "C" {
#endif

	/**
	*  ファイルを開きます
	*  引数:
	*    const void *  - ailiaOpenStreamExもしくはailiaOpenWeightExに与えたfopen_args
	*  返値:
	*    成功した場合、ユーザ定義ファイルポインタを返す。
	*    失敗した場合、NULLを返す。
	*/
	typedef void* (AILIA_USER_API *AILIA_USER_API_FOPEN) (const void *);

	/**
	*  ファイルをシークします
	*  引数:
	*    void *        - ユーザ定義ファイルポインタ
	*    const char *  - ファイル名
	*    long long     - ファイル先頭からのオフセットバイト
	*  返値:
	*    成功した場合、AILIA_USER_API_SUCCESSを返す。
	*    失敗した場合、AILIA_USER_API_FAILEDを返す。
	*/
	typedef int (AILIA_USER_API *AILIA_USER_API_FSEEK) (void*, long long);

	/**
	*  ファイルの現在位置を取得します
	*  引数:
	*    void * - ユーザ定義ファイルポインタ
	*  返値:
	*    成功した場合、ファイルポインタの位置をバイト単位で返す。
	*    失敗した場合、-1を返す。
	*/
	typedef long long (AILIA_USER_API *AILIA_USER_API_FTELL) (void*);

	/**
	*  ファイルのサイズを取得します
	*  引数:
	*    void * - ユーザ定義ファイルポインタ
	*  返値:
	*    成功した場合、ファイルのサイズをバイト単位で返す。
	*    失敗した場合、-1を返す。
	*/
	typedef long long (AILIA_USER_API *AILIA_USER_API_FSIZE) (void*);

	/**
	*  ファイルからデータを読み込みます
	*  引数:
	*    void *                 - 読み込みデータ格納先のポインタ
	*    long long              - 読み込みデータのバイト長さ
	*    void *                 - ユーザ定義ファイルポインタ
	*  返値:
	*    成功した場合、AILIA_USER_API_SUCCESSを返す。
	*    失敗した場合、AILIA_USER_API_FAILEDを返す。
	*    標準APIとは異なり、返値はAILIA_USER_API_*になりますのでご注意ください。
	*/
	typedef int (AILIA_USER_API *AILIA_USER_API_FREAD) (void *, long long, void*);

	/**
	*  ファイルを閉じます
	*  引数:
	*    void *  - ユーザ定義ファイルポインタ
	*  返値:
	*    成功した場合、AILIA_USER_API_SUCCESSを返す。
	*    失敗した場合、AILIA_USER_API_FAILEDを返す。
	*/
	typedef int (AILIA_USER_API *AILIA_USER_API_FCLOSE) (void*);

#define AILIA_FILE_CALLBACK_VERSION (1) /* 構造体バージョン */

	/* ファイルアクセスコールバック関数構造体 */
	typedef struct _ailiaFileCallback {
		AILIA_USER_API_FOPEN  fopen;     /* ユーザ定義fopen関数 */
		AILIA_USER_API_FSEEK  fseek;     /* ユーザ定義fseek関数 */
		AILIA_USER_API_FTELL  ftell;     /* ユーザ定義ftell関数 */
		AILIA_USER_API_FREAD  fread;     /* ユーザ定義fread関数 */
		AILIA_USER_API_FSIZE  fsize;     /* ユーザ定義fsize関数 */
		AILIA_USER_API_FCLOSE fclose;    /* ユーザ定義fclose関数 */
	} ailiaFileCallback;



#ifdef __cplusplus
}
#endif
#endif /* !defined(INCLUDED_AILIA_CALL) */
