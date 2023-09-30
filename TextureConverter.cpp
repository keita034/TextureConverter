#include "TextureConverter.h"

#include<Windows.h>
#include<filesystem>

void TextureConverter::ConvertTextureWICToDDS(const std::string& filePath)
{
	//テクスチャファイルを読み込む
	LoadWICTextureFromFile(filePath);
}

void TextureConverter::LoadWICTextureFromFile(const std::string& filePath)
{
	//ファイルパスをワイド文字列に変換する
	std::wstring wfilePath = ConvertMultiByteStrigToWideString(filePath);
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
