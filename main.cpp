#include<cstdio>
#include<cstdlib>
#include<cassert>
#include"TextureConverter.h"

enum Argument
{
	kApplicationPath,//アプリケーションのパス
	kFilePath,//渡されたファイルのパス

	NumArgument
};

int main(int argc, char* argv[])
{
	//コマンドライン引数指定なし
	if (argc < NumArgument)
	{
		//使い方を表示する
		TextureConverter::OutputUsage();
		return 0;
	}

	//COMライブラリの初期化
	HRESULT hr = CoInitializeEx(nullptr,COINITBASE_MULTITHREADED);
	assert(SUCCEEDED(hr));

	//テクスチャコンバータ
	TextureConverter converter;

	//オプションの数
	int numOptions = argc - NumArgument;
	//オプション配列(ダブルポインタ)
	char** options = argv + NumArgument;

	//テスクチャ変換
	converter.ConvertTextureWICToDDS(argv[kFilePath], numOptions, options);

	//COMライブラリの終了
	CoUninitialize();

	return 0;
}