#include<cstdio>
#include<cstdlib>
#include<cassert>
#include"TextureConverter.h"

enum Argument
{
	kApplicationPath,//�A�v���P�[�V�����̃p�X
	kFilePath,//�n���ꂽ�t�@�C���̃p�X

	NumArgument
};

int main(int argc, char* argv[])
{
	//�R�}���h���C�������w��Ȃ�
	if (argc < NumArgument)
	{
		//�g������\������
		TextureConverter::OutputUsage();
		return 0;
	}

	//COM���C�u�����̏�����
	HRESULT hr = CoInitializeEx(nullptr,COINITBASE_MULTITHREADED);
	assert(SUCCEEDED(hr));

	//�e�N�X�`���R���o�[�^
	TextureConverter converter;

	//�I�v�V�����̐�
	int numOptions = argc - NumArgument;
	//�I�v�V�����z��(�_�u���|�C���^)
	char** options = argv + NumArgument;

	//�e�X�N�`���ϊ�
	converter.ConvertTextureWICToDDS(argv[kFilePath], numOptions, options);

	//COM���C�u�����̏I��
	CoUninitialize();

	return 0;
}