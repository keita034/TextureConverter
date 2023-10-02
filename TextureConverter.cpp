#include "TextureConverter.h"

#include<Windows.h>
#include<filesystem>

void TextureConverter::SaveDDSTextureToFile(int32_t numOptions, char* options[])
{
	HRESULT result;

	size_t mipLevel = 0;
	//ミップマップレベル指定を検索
	for (int i = 0; i < numOptions; i++)
	{
		if (std::string(options[i]) == "-ml")
		{
			//ミップレベル指定
			mipLevel = std::stoi(options[i + 1]);
			break;
		}
	}

	DirectX::ScratchImage mipChain;
	//ミップマップ生成
	result = DirectX::GenerateMipMaps(scratchImage_.GetImages(), scratchImage_.GetImageCount(), scratchImage_.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, mipLevel, mipChain);
	if (SUCCEEDED(result))
	{
		//イメージとメタデータを、ミップマップ版で置き換える
		scratchImage_ = std::move(mipChain);
		metadata_ = scratchImage_.GetMetadata();
	}

	//圧縮形式に変換
	DirectX::ScratchImage converted;
	result = DirectX::Compress(scratchImage_.GetImages(), scratchImage_.GetImageCount(), metadata_, DXGI_FORMAT_BC7_UNORM_SRGB, DirectX::TEX_COMPRESS_BC7_QUICK | DirectX::TEX_COMPRESS_SRGB_OUT | DirectX::TEX_COMPRESS_PARALLEL, 1.0f, converted);
	if (SUCCEEDED(result))
	{
		scratchImage_ = std::move(converted);
		metadata_ = scratchImage_.GetMetadata();
	}

	//読み込んだテクスチャをSRGBとして扱う
	metadata_.format = DirectX::MakeSRGB(metadata_.format);

	//出力ファイル名を設定する
	std::wstring filePath = directoryPath_ + fileName_ + L".dds";

	//DDSファイル書き出し
	result = DirectX::SaveToDDSFile(scratchImage_.GetImages(), scratchImage_.GetImageCount(), metadata_, DirectX::DDS_FLAGS_NONE, filePath.c_str());
	assert(SUCCEEDED(result));
}

void TextureConverter::OutputUsage()
{
	printf("画像ファイルをWIC形式からDDS形式に変換します。\n");
	printf("\n"); //空白行
	printf("TextureConverter [ドライブ:][パス][ファイル名]\n");
	printf("\n"); //空白行
	printf(" [ドライブ:][パス][ファイル名]: 変換したいWIC形式の画像ファイルを指定します。\n");
	printf("\n");
	printf("[-ml level]: ミップレベルを指定します。0を指定すると1x1までのフルミップマップチェーンを生成します。");
}

void TextureConverter::ConvertTextureWICToDDS(const std::string& filePath, int32_t numOptions, char* options[])
{
	//テクスチャファイルを読み込む
	LoadWICTextureFromFile(filePath);

	SaveDDSTextureToFile(numOptions, options);
}

void TextureConverter::LoadWICTextureFromFile(const std::string& filePath)
{
	//ファイルパスをワイド文字列に変換する
	std::wstring wfilePath = ConvertMultiByteStrigToWideString(filePath);

	HRESULT result = DirectX::LoadFromWICFile(wfilePath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata_, scratchImage_);
	assert(SUCCEEDED(result));

	//フォルダパスとファイル名を分離する
	SeparateFilePath(wfilePath);
}

std::wstring TextureConverter::ConvertMultiByteStrigToWideString(const std::string& mString)
{
	auto const dest_size = ::MultiByteToWideChar(CP_ACP, 0U, mString.data(), -1, nullptr, 0U);
	std::vector<wchar_t> dest(static_cast<const uint64_t>(dest_size), L'\0');

	if (::MultiByteToWideChar(CP_ACP, 0U, mString.data(), -1, dest.data(), static_cast<int32_t>(dest.size())) == 0)
	{
		throw std::system_error{ static_cast<int32_t>(::GetLastError()), std::system_category() };
	}

	dest.resize(std::char_traits<wchar_t>::length(dest.data()));
	dest.shrink_to_fit();

	return std::wstring(dest.begin(), dest.end());
}

void TextureConverter::SeparateFilePath(const std::wstring& filePath)
{
	size_t pos1 = 0;
	std::wstring exceptExt;

	//区切り文字'.'が出てくる一番最後の部分を検索
	pos1 = filePath.rfind('.');
	//検索がヒットしたら
	if (pos1 != std::wstring::npos)
	{
		//区切り文字の後ろをファイル拡張子として保存
		fileExt_ = filePath.substr(pos1 + 1, filePath.size() - pos1 - 1);
		//区切り文字の前までを抜き出す
		exceptExt = filePath.substr(0, pos1);
	}
	else
	{
		fileExt_ = L"";
		exceptExt = filePath;
	}

	//区切り文字'\\'が出てくる一番最後の部分を検索
	pos1 = exceptExt.rfind('\\');
	if (pos1 != std::wstring::npos)
	{
		//区切り文字の前までをディレクトリパスとして保存
		directoryPath_ = exceptExt.substr(0, pos1 + 1);
		//区切り文字の後ろをファイル名として保存
		fileName_ = exceptExt.substr(pos1 + 1, exceptExt.size() - pos1 - 1);
		return;
	}

	//区切り文字'\\'が出てくる一番最後の部分を検索
	pos1 = exceptExt.rfind('/');
	if (pos1 != std::wstring::npos)
	{
		//区切り文字の前までをディレクトリパスとして保存
		directoryPath_ = exceptExt.substr(0, pos1 + 1);
		//区切り文字の後ろをファイル名として保存
		fileName_ = exceptExt.substr(pos1 + 1, exceptExt.size() - pos1 - 1);
		return;
	}

	//区切り文字がないのでファイル名のみとして扱う
	directoryPath_ = L"";
	fileName_ = exceptExt;
}
