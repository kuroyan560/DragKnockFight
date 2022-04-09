#include "D3D12App.h"
#include"KuroFunc.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

D3D12App* D3D12App::INSTANCE = nullptr;
std::map<std::string, D3D12App::LoadLambda_t> D3D12App::loadLambdaTable;

void D3D12App::Initialize(const HWND& Hwnd, const Vec2<int>& ScreenSize, const bool& UseHDR, const Color& ClearValue, const bool& IsFullScreen)
{
//�f�o�b�O���C���[
#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	HRESULT result;

	//�O���t�B�b�N�X�{�[�h�̃A�_�v�^���
	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	//�A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter>>adapters;
	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter> tmpAdapter = nullptr;
	for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		result = dxgiFactory->EnumAdapters(i, &tmpAdapter);
		adapters.push_back(tmpAdapter);		//���I�z��ɒǉ�����
	}
	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC adesc{};
		result = adapters[i]->GetDesc(&adesc);	//�A�_�v�^�[�̏����擾
		std::wstring strDesc = adesc.Description;	//�A�_�v�^�[��
		//Microsoft Basic Render Driver,Intel UHD Graphics �����
		if (strDesc.find(L"Microsoft") == std::wstring::npos
			&& strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i];	//�̗p
			break;
		}
	}

	//�f�o�C�X�̐����iDirect3D12�̊�{�I�u�W�F�N�g�j
	//�Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&device));
		if (result == S_OK)
		{
			//�f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}

	descHeapCBV_SRV_UAV = std::make_unique<DescriptorHeap_CBV_SRV_UAV>(device);
	descHeapRTV = std::make_unique<DescriptorHeap_RTV>(device);
	descHeapDSV = std::make_unique<DescriptorHeap_DSV>(device);

	//�R�}���h�A���P�[�^�𐶐�
	commandAllocators.resize(FRAME_BUFFER_COUNT);
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		result = device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocators[i])
		);
		if (FAILED(result))
		{
			printf("�R�}���h�A���P�[�^�̐������s\n");
			assert(0);
		}
	}
	//�o�b�t�@�̓]�����s�����߂ɃR�}���h���X�g���g���̂ŏ���
	commandAllocators[0]->Reset();

	//�R�}���h���X�g�𐶐��iGPU�ɁA�܂Ƃ߂Ė��߂𑗂邽�߂̃R�}���h���X�g�𐶐�����j
	result = device->CreateCommandList(
		0, 
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocators[0].Get(),
		nullptr,
		IID_PPV_ARGS(&commandList));

	//�R�}���h�L���[�̐����i�R�}���h���X�g��GPU�ɏ��Ɏ��s�����Ă����ׂ̎d�g�݂𐶐�����j
	//�W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC queueDesc{
	  D3D12_COMMAND_LIST_TYPE_DIRECT,
	  0,
	  D3D12_COMMAND_QUEUE_FLAG_NONE,
	  0
	};
	result = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

	// �X���b�v�`�F�C���̐���
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc{};
		scDesc.BufferCount = FRAME_BUFFER_COUNT;
		scDesc.Width = ScreenSize.x;
		scDesc.Height = ScreenSize.y;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		scDesc.SampleDesc.Count = 1;
		//scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // �f�B�X�v���C�̉𑜓x���ύX����ꍇ�ɂ̓R�����g�����B

		//HDR�Ή���
		bool useHDR = UseHDR;
		if (useHDR)
		{
			bool isDisplayHDR10 = false;
			UINT index = 0;
			ComPtr<IDXGIOutput>current;
			while (tmpAdapter->EnumOutputs(index, &current) != DXGI_ERROR_NOT_FOUND)
			{
				ComPtr<IDXGIOutput6>output6;
				current.As(&output6);

				DXGI_OUTPUT_DESC1 desc;
				output6->GetDesc1(&desc);
				isDisplayHDR10 |= desc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
				++index;
			}
			if (!isDisplayHDR10)useHDR = false;
		}
		if (useHDR)scDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		else scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		backBuffFormat = scDesc.Format;	//�o�b�N�o�b�t�@�̃t�H�[�}�b�g��ۑ����Ă���

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		fsDesc.Windowed = IsFullScreen ? FALSE : TRUE;
		fsDesc.RefreshRate.Denominator = 1000;
		fsDesc.RefreshRate.Numerator = 60317;
		fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;

		ComPtr<IDXGISwapChain1> swapchain1;
		result = dxgiFactory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			Hwnd,
			&scDesc,
			&fsDesc,
			nullptr,
			&swapchain1);

		if (FAILED(result))
		{
			printf("�X���b�v�`�F�C���������s\n");
			assert(0);
		}

		auto val = ClearValue.GetColor();
		float clearValue[4] =
		{
			val.x,
			val.y,
			val.z,
			val.w,
		};
		swapchain = std::make_unique<Swapchain>(device, swapchain1, *descHeapCBV_SRV_UAV, *descHeapRTV, useHDR, clearValue);
	}

	//�摜���[�h�̃����_������
	loadLambdaTable["sph"]
		= loadLambdaTable["spa"]
		= loadLambdaTable["bmp"]
		= loadLambdaTable["png"]
		= loadLambdaTable["jpg"]
		= [](const std::wstring& Path, TexMetadata* Meta, ScratchImage& Img)->HRESULT
	{
		return LoadFromWICFile(Path.c_str(), WIC_FLAGS_NONE, Meta, Img);
	};
	loadLambdaTable["tga"]
		= [](const std::wstring& Path, TexMetadata* Meta, ScratchImage& Img)->HRESULT
	{
		return LoadFromTGAFile(Path.c_str(), Meta, Img);
	};
	loadLambdaTable["dds"]
		= [](const std::wstring& Path, TexMetadata* Meta, ScratchImage& Img)->HRESULT
	{
		return LoadFromDDSFile(Path.c_str(), DDS_FLAGS_NONE, Meta, Img);
	};

	//�摜�𕪊�����p�C�v���C��
	{
		//�V�F�[�_
		auto cs = CompileShader("resource/HLSL/Engine/RectTexture.hlsl", "CSmain", "cs_5_0");
		//���[�g�p�����[�^
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"������̉摜"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�����O�̉摜"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�����ԍ�")
		};
		//�p�C�v���C������
		splitImgPipeline = GenerateComputePipeline(cs, rootParams, WrappedSampler(true, false));
	}
}

std::shared_ptr<VertexBuffer> D3D12App::GenerateVertexBuffer(const size_t& VertexSize, const int& VertexNum, void* InitSendData, const char* Name, const bool& RWStructuredBuff)
{
	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//���_�o�b�t�@�T�C�Y
	UINT sizeVB = static_cast<UINT>(VertexSize * VertexNum);

	D3D12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	if (RWStructuredBuff)
	{
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		//�q�[�v�v���p�e�B
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		prop.CreationNodeMask = 1;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		prop.Type = D3D12_HEAP_TYPE_CUSTOM;
		prop.VisibleNodeMask = 1;
	}

	//���_�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	if (FAILED(hr))ASSERT_MSG("���_�o�b�t�@�̐����Ɏ��s\n");

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//���_�o�b�t�@�r���[�쐬
	D3D12_VERTEX_BUFFER_VIEW vbView;
	vbView.BufferLocation = buff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = VertexSize;

	//��p�̃N���X�ɂ܂Ƃ߂�
	std::shared_ptr<VertexBuffer>result;

	//�ǂݎ���p�\�����o�b�t�@�𐶐����邩
	if (RWStructuredBuff)
	{
		//�V�F�[�_���\�[�X�r���[�쐬
		descHeapCBV_SRV_UAV->CreateUAV(device, buff, VertexSize, VertexNum);

		//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
		DescHandles handles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

		result = std::make_shared<VertexBuffer>(buff, barrier, vbView, handles);
	}
	else
	{
		result = std::make_shared<VertexBuffer>(buff, barrier, vbView);
	}

	//�������}�b�s���O
	if (InitSendData != nullptr)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<IndexBuffer> D3D12App::GenerateIndexBuffer(const int& IndexNum, void* InitSendData, const char* Name, const DXGI_FORMAT& IndexFormat)
{
	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	size_t indexSize = 0;
	if (IndexFormat == DXGI_FORMAT_R32_UINT)indexSize = sizeof(unsigned int);
	else if (IndexFormat == DXGI_FORMAT_R16_UINT)indexSize = sizeof(unsigned short);
	else ASSERT_MSG("�C���f�b�N�X�o�b�t�@�𐶐����悤�Ƃ��܂������A�Ή����Ă��Ȃ��t�H�[�}�b�g�ł�\n");

	//�C���f�b�N�X�o�b�t�@�T�C�Y
	UINT sizeIB = static_cast<UINT>(indexSize * IndexNum);

	//���_�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	if (FAILED(hr))ASSERT_MSG("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//�C���f�b�N�X�o�b�t�@�r���[�쐬
	D3D12_INDEX_BUFFER_VIEW ibView;
	ibView.BufferLocation = buff->GetGPUVirtualAddress();
	ibView.Format = IndexFormat;
	ibView.SizeInBytes = sizeIB;

	//��p�̃N���X�ɂ܂Ƃ߂�
	auto result = std::make_shared<IndexBuffer>(buff, barrier, ibView, indexSize);

	//�������}�b�s���O
	if (InitSendData != nullptr)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<ConstantBuffer> D3D12App::GenerateConstantBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData, const char* Name)
{
	//�A���C�����g�����T�C�Y
	size_t alignmentSize = (static_cast<UINT>(DataSize * ElementNum) + 0xff) & ~0xff;

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//�萔�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(alignmentSize),
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	if (FAILED(hr))ASSERT_MSG("�萔�o�b�t�@�̐����Ɏ��s\n");

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//�萔�o�b�t�@�r���[�쐬
	descHeapCBV_SRV_UAV->CreateCBV(device, buff->GetGPUVirtualAddress(), alignmentSize);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̒萔�o�b�t�@�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<ConstantBuffer>result;
	result = std::make_shared<ConstantBuffer>(buff, barrier, handles, DataSize, ElementNum);

	//�����l�}�b�s���O
	if (InitSendData != nullptr)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<StructuredBuffer> D3D12App::GenerateStructuredBuffer(const size_t& DataSize, const int& ElementNum, void* InitSendData, const char* Name)
{
	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//�萔�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(DataSize * ElementNum),
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	if (FAILED(hr))ASSERT_MSG("�\�����o�b�t�@�̐����Ɏ��s\n");

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//�V�F�[�_���\�[�X�r���[�쐬
	descHeapCBV_SRV_UAV->CreateSRV(device, buff, DataSize, ElementNum);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̍\�����o�b�t�@�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<StructuredBuffer>result;
	result = std::make_shared<StructuredBuffer>(buff, barrier, handles, DataSize, ElementNum);

	//�����l�}�b�s���O
	if (InitSendData != nullptr)result->Mapping(InitSendData);

	return result;
}

std::shared_ptr<RWStructuredBuffer> D3D12App::GenerateRWStructuredBuffer(const size_t& DataSize, const int& ElementNum, const char* Name)
{
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(DataSize * ElementNum);
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	//�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;

	//�萔�o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	if (FAILED(hr))ASSERT_MSG("�o�͗p�o�b�t�@�̐����Ɏ��s\n");

	//���O�̃Z�b�g
	if (Name != nullptr)buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());

	//�V�F�[�_���\�[�X�r���[�쐬
	descHeapCBV_SRV_UAV->CreateUAV(device, buff, DataSize, ElementNum);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̍\�����o�b�t�@�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<RWStructuredBuffer>result;
	result = std::make_shared<RWStructuredBuffer>(buff, barrier, handles, DataSize, ElementNum);

	return result;
}

std::shared_ptr<TextureBuffer> D3D12App::GenerateTextureBuffer(const Color& Color, const DXGI_FORMAT& Format, const int& Width)
{
	//���ɂ��邩�m�F
	for (auto itr = colorTextures.begin(); itr != colorTextures.end(); ++itr)
	{
		if (itr->color == Color && itr->width == Width)
		{
			return itr->tex;
		}
	}

	//�Ȃ������̂Ő�������
	const int texDataCount = Width * Width;

	//�e�N�X�`���f�[�^�z��
	XMFLOAT4* texturedata = new XMFLOAT4[texDataCount];

	//�S�s�N�Z���̐F��������
	auto colorval = Color.GetColor();
	for (int i = 0; i < texDataCount; ++i)
	{
		texturedata[i].x = colorval.x;	//R
		texturedata[i].y = colorval.y;	//G
		texturedata[i].z = colorval.z;	//B
		texturedata[i].w = colorval.w;	//A
	}

	//�e�N�X�`���q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//�e�N�X�`�����\�[�X�ݒ�
	D3D12_RESOURCE_DESC texDesc{};	//���\�[�X�ݒ�
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//2D�e�N�X�`���p
	texDesc.Format = Format;	//RGBA�t�H�[�}�b�g
	texDesc.Width = Width;
	texDesc.Height = Width;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//�e�N�X�`���p���\�[�X�o�b�t�@�̐���
	ComPtr<ID3D12Resource1>buff;
	auto hr = device->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));

	if (FAILED(hr))ASSERT_MSG("�P�F�h��Ԃ��e�N�X�`���o�b�t�@�̐����Ɏ��s\n");

	//�o�b�t�@�ɖ��O�Z�b�g
	std::wstring name = L"ColorTexture - ";
	name += std::to_wstring(colorval.x) + L" , ";
	name += std::to_wstring(colorval.y) + L" , ";
	name += std::to_wstring(colorval.z) + L" , ";
	name += std::to_wstring(colorval.w);
	buff->SetName(name.c_str());

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	hr = buff->WriteToSubresource(
		0,
		nullptr,	//�S�̈�փR�s�[
		texturedata,	//���f�[�^�A�h���X
		sizeof(XMFLOAT4) * Width,	//1���C���T�C�Y
		sizeof(XMFLOAT4) * texDataCount	//�P���T�C�Y
	);
	delete[] texturedata;

	if (FAILED(hr))ASSERT_MSG("�P�F�h��Ԃ��e�N�X�`���o�b�t�@�ւ̃f�[�^�]���Ɏ��s\n");

	//�V�F�[�_�[���\�[�X�r���[�쐬
	descHeapCBV_SRV_UAV->CreateSRV(device, buff, Format);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̃V�F�[�_�[���\�[�X�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<TextureBuffer>result;
	result = std::make_shared<TextureBuffer>(buff, barrier, handles, texDesc);
	
	//�e�N�X�`���p�̃��\�[�X�o���A�ɕύX
	result->ChangeBarrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//�쐬�����J���[�e�N�X�`�������L�^
	ColorTexture colorTexData;
	colorTexData.color = Color;
	colorTexData.width = Width;
	colorTexData.tex = result;
	colorTextures.emplace_back(colorTexData);

	return result;
}

std::shared_ptr<TextureBuffer> D3D12App::GenerateTextureBuffer(const std::string& LoadImgFilePath)
{
	//���ɂ��邩�m�F
	for (auto itr = loadImgTextures.begin(); itr != loadImgTextures.end(); ++itr)
	{
		if (itr->path == LoadImgFilePath)
		{
			return itr->textures[0];
		}
	}

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	//���C�h�����ϊ�
	auto wtexpath = KuroFunc::GetWideStrFromStr(LoadImgFilePath);

	//�g���q�擾
	auto ext = KuroFunc::GetExtension(LoadImgFilePath);

	//���[�h
	auto hr = loadLambdaTable[ext](
		wtexpath,
		&metadata,
		scratchImg);
	if (FAILED(hr))ASSERT_MSG("�摜�f�[�^���o�Ɏ��s\n");

	const Image* img = scratchImg.GetImage(0, 0, 0);	//���f�[�^���o

	//�e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,	//RGBA�t�H�[�}�b�g
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_GENERIC_READ;

	//�e�N�X�`���p���\�[�X�o�b�t�@�̐���
	ComPtr<ID3D12Resource1>buff;
	CD3DX12_HEAP_PROPERTIES heapPropForTex(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	hr = device->CreateCommittedResource(
		&heapPropForTex,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));
	if (FAILED(hr))ASSERT_MSG("���[�h�摜�e�N�X�`���o�b�t�@�����Ɏ��s\n");

	//���O�Z�b�g
	buff->SetName(wtexpath.c_str());

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	hr = buff->WriteToSubresource(
		0,
		nullptr,	//�S�̈�փR�s�[
		img->pixels,	//���f�[�^�A�h���X
		(UINT)img->rowPitch,	//1���C���T�C�Y
		(UINT)img->slicePitch	//�P���T�C�Y
	);
	if (FAILED(hr))ASSERT_MSG("���[�h�摜�e�N�X�`���o�b�t�@�ւ̃f�[�^�]���Ɏ��s");

	//�V�F�[�_�[���\�[�X�r���[�쐬
	descHeapCBV_SRV_UAV->CreateSRV(device, buff, metadata.format);

	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̃V�F�[�_�[���\�[�X�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<TextureBuffer>result;
	result = std::make_shared<TextureBuffer>(buff, barrier, handles, texDesc);

	//�e�N�X�`���p�̃��\�[�X�o���A�ɕύX
	result->ChangeBarrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//�쐬�����J���[�e�N�X�`�������L�^
	LoadImgTexture loadImgTexData;
	loadImgTexData.path = LoadImgFilePath;
	loadImgTexData.textures = { result };
	loadImgTextures.emplace_back(loadImgTexData);

	return result;
}

std::shared_ptr<TextureBuffer> D3D12App::GenerateTextureBuffer(const Vec2<int>& Size, const DXGI_FORMAT& Format, const char* Name)
{
	//�e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		Format,	//RGBA�t�H�[�}�b�g
		(UINT)Size.x,
		(UINT)Size.y,
		(UINT16)1,
		(UINT16)1);
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	//�e�N�X�`���p���\�[�X�o�b�t�@�̐���
	ComPtr<ID3D12Resource1>buff;
	CD3DX12_HEAP_PROPERTIES heapPropForTex(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto hr = device->CreateCommittedResource(
		&heapPropForTex,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		barrier,
		nullptr,
		IID_PPV_ARGS(&buff));
	if (FAILED(hr))ASSERT_MSG("�����摜�p�e�N�X�`���o�b�t�@�����Ɏ��s\n");

	//���O�Z�b�g
	if (Name != nullptr)
	{
		buff->SetName(KuroFunc::GetWideStrFromStr(Name).c_str());
	}

	//�V�F�[�_�[���\�[�X�r���[�쐬
	descHeapCBV_SRV_UAV->CreateSRV(device, buff, texDesc.Format);
	DescHandles srvHandles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//�A���I�[�_�[�h�A�N�Z�X�r���[�쐬
	descHeapCBV_SRV_UAV->CreateUAV(device, buff, 4, texDesc.Width * texDesc.Height, D3D12_UAV_DIMENSION_TEXTURE2D, texDesc.Format);
	DescHandles uavHandles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//��p�̃V�F�[�_�[���\�[�X�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<TextureBuffer>result;
	result = std::make_shared<TextureBuffer>(buff, barrier, srvHandles, texDesc);
	result->SetUAVHandles(uavHandles);

	return result;
}

std::vector<std::shared_ptr<TextureBuffer>> D3D12App::GenerateTextureBuffer(const std::string& LoadImgFilePath, const int& AllNum, const Vec2<int>& SplitNum)
{
	auto sourceTexture = GenerateTextureBuffer(LoadImgFilePath);
	return SplitTextureBuffer(sourceTexture, AllNum, SplitNum, LoadImgFilePath);
}

void D3D12App::GenerateTextureBuffer(std::shared_ptr<TextureBuffer>* Array, const std::string& LoadImgFilePath, const int& AllNum, const Vec2<int>& SplitNum)
{
	auto sourceTexture = GenerateTextureBuffer(LoadImgFilePath);
	return SplitTextureBuffer(Array, sourceTexture, AllNum, SplitNum, LoadImgFilePath);
}

std::shared_ptr<RenderTarget> D3D12App::GenerateRenderTarget(const DXGI_FORMAT& Format, const Color& ClearValue, const Vec2<int> Size, const wchar_t* TargetName, D3D12_RESOURCE_STATES InitState, int MipLevel, int ArraySize)
{
	//�����_�[�^�[�Q�b�g�ݒ�
	CD3DX12_RESOURCE_DESC desc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		static_cast<UINT>(Size.x),
		static_cast<UINT>(Size.y),
		ArraySize,
		MipLevel,
		Format,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	//�����_�[�^�[�Q�b�g�̃N���A�l
	auto colorval = ClearValue.GetColor();
	float clearColor[4] =
	{
		colorval.x,
		colorval.y,
		colorval.z,
		colorval.w
	};

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = Format;
	for (int i = 0; i < 4; ++i)
	{
		clearValue.Color[i] = clearColor[i];
	}

	//���\�[�X����
	ComPtr<ID3D12Resource1>buff;
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		InitState,
		&clearValue,
		IID_PPV_ARGS(&buff));
	if (FAILED(hr))ASSERT_MSG("�����_�[�^�[�Q�b�g�o�b�t�@�����Ɏ��s\n");

	//���O�Z�b�g
	if (TargetName != nullptr)buff->SetName(TargetName);

	//SRV�쐬
	descHeapCBV_SRV_UAV->CreateSRV(device, buff, Format);
	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles srvHandles(descHeapCBV_SRV_UAV->GetCpuHandleTail(), descHeapCBV_SRV_UAV->GetGpuHandleTail());

	//RTV�쐬
	descHeapRTV->CreateRTV(device, buff);
	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles rtvHandles(descHeapRTV->GetCpuHandleTail(), descHeapRTV->GetGpuHandleTail());

	//��p�̃����_�[�^�[�Q�b�g�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<RenderTarget>result;
	result = std::make_shared<RenderTarget>(buff, InitState, srvHandles, rtvHandles, desc, clearColor);

	return result;
}

std::shared_ptr<DepthStencil> D3D12App::GenerateDepthStencil(const Vec2<int>& Size, const DXGI_FORMAT& Format, const float& ClearValue)
{
	//���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		Format,	//�[�x�l�t�H�[�}�b�g
		static_cast<UINT>(Size.x),
		static_cast<UINT>(Size.y),
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);	//�f�v�X�X�e���V��

	//���\�[�X�o���A
	auto barrier = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	//�f�v�X�X�e���V���o�b�t�@����
	ComPtr<ID3D12Resource1>buff;
	auto hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(Format, ClearValue, 0),
		IID_PPV_ARGS(&buff));
	if (FAILED(hr))ASSERT_MSG("�f�v�X�X�e���V���o�b�t�@�����Ɏ��s\n");

	//DSV�쐬
	descHeapDSV->CreateDSV(device, buff);
	//�r���[���쐬�����ʒu�̃f�B�X�N���v�^�n���h�����擾
	DescHandles handles(descHeapDSV->GetCpuHandleTail(), descHeapDSV->GetGpuHandleTail());

	//��p�̃����_�[�^�[�Q�b�g�N���X�ɂ܂Ƃ߂�
	std::shared_ptr<DepthStencil>result;
	result = std::make_shared<DepthStencil>(buff, barrier, handles, desc, ClearValue);

	return result;
}

void D3D12App::SetDescHeaps()
{
	//�f�B�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* heaps[] = { descHeapCBV_SRV_UAV->GetHeap().Get() };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);
}

void D3D12App::Render(D3D12AppUser* User)
{
	SetDescHeaps();

	//�X���b�v�`�F�C���\���\���烌���_�[�^�[�Q�b�g�`��\��
	swapchain->GetBackBufferRenderTarget()->ChangeBarrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//�����_�[�^�[�Q�b�g���N���A
	swapchain->GetBackBufferRenderTarget()->Clear(commandList);

	//�����_�����O����
	User->Render();

	//�����_�[�^�[�Q�b�g����X���b�v�`�F�C���\���\��
	swapchain->GetBackBufferRenderTarget()->ChangeBarrier(commandList, D3D12_RESOURCE_STATE_PRESENT);

	//���߂̃N���[�Y
	auto hr = commandList->Close();
	if (FAILED(hr))ASSERT_MSG("�R�}���h���X�g���߂̃N���[�Y�Ɏ��s\n");

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { commandList.Get() };	//�R�}���h���X�g�̔z��
	commandQueue->ExecuteCommandLists(1, cmdLists);

	//�o�b�t�@���t���b�v�i���\�̓���ւ��j
	hr = swapchain->GetSwapchain()->Present(1, 0);
	if (FAILED(hr))ASSERT_MSG("�o�b�N�o�b�t�@�̃t���b�v�Ɏ��s\n");

	//�o�b�N�o�b�t�@�ԍ��擾
	auto frameIdx = swapchain->GetSwapchain()->GetCurrentBackBufferIndex();

	//�R�}���h�A���P�[�^���Z�b�g
	hr = commandAllocators[frameIdx]->Reset();	//�L���[���N���A
	if (FAILED(hr))ASSERT_MSG("�R�}���h�A���P�[�^���Z�b�g�Ɏ��s\n");

	//�R�}���h���X�g
	hr = commandList->Reset(commandAllocators[frameIdx].Get(), nullptr);		//�R�}���h���X�g�𒙂߂鏀��
	if (FAILED(hr))ASSERT_MSG("�R�}���h���X�g�̃��Z�b�g�Ɏ��s\n");

	//�R�}���h���X�g�̎��s������҂�
	swapchain->WaitPreviousFrame(commandQueue, frameIdx);

	//SplitTexBuff�Ă΂ꂽ�񐔃��Z�b�g
	splitTexBuffCount = 0;
}

#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
Microsoft::WRL::ComPtr<ID3DBlob>D3D12App::CompileShader(const std::string& FilePath, const std::string& EntryPoint, const std::string& ShaderModel)
{
	ComPtr<ID3DBlob>result;
	ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	//���C�h�����ɕϊ�
	const std::wstring wideFilePath = KuroFunc::GetWideStrFromStr(FilePath);

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	auto hr = D3DCompileFromFile(
		wideFilePath.c_str(),		//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
		EntryPoint.c_str(), ShaderModel.c_str(),	//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		result.GetAddressOf(), &errorBlob);

	//�V�F�[�_�̃G���[���e��\��
	if (FAILED(hr))
	{
		//errorBlob����G���[���estring�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += '\n';
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}
	return result;
}

std::shared_ptr<GraphicsPipeline>D3D12App::GenerateGraphicsPipeline(
	const PipelineInitializeOption& Option,
	const Shaders& ShaderInfo,
	const std::vector<InputLayoutParam>& InputLayout,
	const std::vector<RootParam>& RootParams,
	const std::vector<RenderTargetInfo>& RenderTargetFormat,
	const WrappedSampler& SamplerDesc)
{
	HRESULT hr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

	std::vector<D3D12_INPUT_ELEMENT_DESC>inputLayout;
	//�C���v�b�g���C�A�E�g
	{
		for (auto& param : InputLayout)
		{
			D3D12_INPUT_ELEMENT_DESC input =
			{
				param.semantics.c_str(),	//�Z�}���e�B�b�N��
				0,				//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X�i�O�ł悢�j
				param.format,	//�v�f���ƃr�b�g����\���iXYZ�̂R��float�^�Ȃ̂� R32G32B32_FLOAT)
				0,	//���̓X���b�g�C���f�b�N�X�i�O�ł悢�j
				D3D12_APPEND_ALIGNED_ELEMENT,	//�f�[�^�̃I�t�Z�b�g�l�iD3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�j
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//���̓f�[�^��ʁi�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA�j
				0		//��x�ɕ`�悷��C���X�^���X���i�O�ł悢�j
			};
			inputLayout.emplace_back(input);
		}
		desc.InputLayout.pInputElementDescs = &inputLayout[0];
		desc.InputLayout.NumElements = inputLayout.size();
	}

	//���[�g�p�����[�^
	ComPtr<ID3D12RootSignature>rootSignature;
	{
		std::vector<CD3DX12_ROOT_PARAMETER>rootParameters;
		std::vector< CD3DX12_DESCRIPTOR_RANGE>rangeArray;

		//�e�����W�^�C�v�Ń��W�X�^�[�������o�^���ꂽ��
		int registerNum[D3D12_DESCRIPTOR_RANGE_TYPE_NUM] = { 0 };
		for (auto& param : RootParams)
		{
			//�f�B�X�N���v�^�Ƃ��ď�����
			if (param.descriptor)
			{
				//�^�C�v�̎擾
				auto& type = param.descriptorRangeType;

				//�f�B�X�N���v�^�����W������
				CD3DX12_DESCRIPTOR_RANGE range{};
				range.Init(type, 1, registerNum[(int)type]);

				registerNum[(int)type]++;
				rangeArray.emplace_back(range);
			}
			//�r���[�ŏ�����
			else
			{
				rootParameters.emplace_back();
				if (param.viewType == SRV)
				{
					auto type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
					rootParameters.back().InitAsShaderResourceView(registerNum[(int)type]);
					registerNum[(int)type]++;
				}
				else if (param.viewType == CBV)
				{
					auto type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					rootParameters.back().InitAsConstantBufferView(registerNum[(int)type]);
					registerNum[(int)type]++;
				}
				else if (param.viewType == UAV)
				{
					auto type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
					rootParameters.back().InitAsUnorderedAccessView(registerNum[(int)type]);
					registerNum[(int)type]++;
				}
				else
				{
					ASSERT_MSG("���[�g�p�����[�^����肭�����܂���ł���\n");
				}
			}
		}

		for (int i = 0; i < rangeArray.size(); ++i)
		{
			rootParameters.emplace_back();
			rootParameters.back().InitAsDescriptorTable(1, &rangeArray[i]);
		}

		// ���[�g�V�O�l�`���̐ݒ�
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_0(rootParameters.size(), &rootParameters[0], 1, &SamplerDesc.sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> rootSigBlob;
		ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
		// �o�[�W������������̃V���A���C�Y
		auto hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
		if (FAILED(hr))
		{
			//errorBlob����G���[���estring�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += '\n';
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			ASSERT_MSG("���[�g�V�O�l�`���̃o�[�W������������̃V���A���C�Y���s\n");
		}

		// ���[�g�V�O�l�`���̐���
		hr = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

		//���[�g�V�O�l�`�������Ɏ��s
		if (FAILED(hr))ASSERT_MSG("���[�g�V�O�l�`���̐����Ɏ��s\n");
	}

	//�O���t�B�b�N�X�p�C�v���C���ݒ�Ƀ��[�g�V�O�l�`�����Z�b�g
	desc.pRootSignature = rootSignature.Get();

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	{
		//�T���v���}�X�N
		desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

		// ���X�^���C�U�X�e�[�g
		desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//�J�����O���[�h
		if (!Option.calling)
		{
			desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		}
		//���C���[�t���[��
		if (Option.wireFrame)
		{
			desc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		}
		//�[�x�e�X�g
		if (Option.depthTest)
		{
			desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			//�f�v�X�̏������݂��֎~�i�[�x�e�X�g�͍s���j
			if (!Option.depthWriteMask)
			{
				desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			}
		}
		//�f�v�X�X�e���V���o�b�t�@�t�H�[�}�b�g
		desc.DSVFormat = Option.dsvFormat;

		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//���������_�[�^�[�Q�b�g�œƗ������u�����f�B���O��L���ɂ��邩
		desc.BlendState.IndependentBlendEnable = Option.independetBlendEnable;

		// 1�s�N�Z���ɂ�1��T���v�����O
		desc.SampleDesc.Count = 1;

		// �}�`�̌`��ݒ�
		desc.PrimitiveTopologyType = Option.primitiveTopologyType;

		//�������ݐ惌���_�[�^�[�Q�b�g
		desc.NumRenderTargets = 0;
		for (auto& info : RenderTargetFormat)
		{
			int idx = ++desc.NumRenderTargets - 1;
			if (D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT < desc.NumRenderTargets)
			{
				ASSERT_MSG("�`��惌���_�[�^�[�Q�b�g�̐����ő�𒴂��Ă��܂�\n");
			}

			//�`��惌���_�[�^�[�Q�b�g�̃t�H�[�}�b�g
			desc.RTVFormats[idx] = info.format;

			//�A���t�@�u�����f�B���O�ݒ�
			if (info.blendMode == AlphaBlendMode_Trans)	//����������
			{
				desc.BlendState.RenderTarget[idx].BlendEnable = true;
				desc.BlendState.RenderTarget[idx].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				desc.BlendState.RenderTarget[idx].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				desc.BlendState.RenderTarget[idx].BlendOp = D3D12_BLEND_OP_ADD;
			}
			else if (info.blendMode == AlphaBlendMode_Add)	//���Z����
			{
				//���Z�����̃u�����h�X�e�[�g�쐬
				desc.BlendState.RenderTarget[idx].BlendEnable = true;
				desc.BlendState.RenderTarget[idx].SrcBlend = D3D12_BLEND_ONE;
				desc.BlendState.RenderTarget[idx].DestBlend = D3D12_BLEND_ONE;
				desc.BlendState.RenderTarget[idx].BlendOp = D3D12_BLEND_OP_ADD;
			}
			else desc.BlendState.RenderTarget[idx].BlendEnable = false;	//���S�㏑��
		}
	}

	//�V�F�[�_�[�I�u�W�F�N�g�Z�b�g
	if(ShaderInfo.vs.Get())desc.VS = CD3DX12_SHADER_BYTECODE(ShaderInfo.vs.Get());
	if(ShaderInfo.ps.Get())desc.PS = CD3DX12_SHADER_BYTECODE(ShaderInfo.ps.Get());
	if(ShaderInfo.ds.Get())desc.DS = CD3DX12_SHADER_BYTECODE(ShaderInfo.ds.Get());
	if(ShaderInfo.hs.Get())desc.HS = CD3DX12_SHADER_BYTECODE(ShaderInfo.hs.Get());
	if(ShaderInfo.gs.Get())desc.GS = CD3DX12_SHADER_BYTECODE(ShaderInfo.gs.Get());

	//�O���t�B�b�N�X�p�C�v���C���̐���
	ComPtr<ID3D12PipelineState>pipeline;
	hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline));
	if (FAILED(hr))ASSERT_MSG("�O���t�B�b�N�X�p�C�v���C�������Ɏ��s\n");

	return std::make_shared<GraphicsPipeline>(pipeline, rootSignature, Option.primitiveTopology);
}

std::shared_ptr<ComputePipeline> D3D12App::GenerateComputePipeline(const ComPtr<ID3DBlob>& ComputeShader, const std::vector<RootParam>& RootParams, const WrappedSampler& Sampler)
{
	HRESULT hr;

	// �p�C�v���C���X�e�[�g���쐬
	D3D12_COMPUTE_PIPELINE_STATE_DESC  desc = { 0 };

	//���[�g�p�����[�^
	ComPtr<ID3D12RootSignature>rootSignature;
	{
		std::vector<CD3DX12_ROOT_PARAMETER>rootParameters;
		std::vector< CD3DX12_DESCRIPTOR_RANGE>rangeArray;

		//�e�����W�^�C�v�Ń��W�X�^�[�������o�^���ꂽ��
		int registerNum[D3D12_DESCRIPTOR_RANGE_TYPE_NUM] = { 0 };
		for (auto& param : RootParams)
		{
			//�f�B�X�N���v�^�Ƃ��ď�����
			if (param.descriptor)
			{
				//�^�C�v�̎擾
				auto& type = param.descriptorRangeType;

				//�f�B�X�N���v�^�����W������
				CD3DX12_DESCRIPTOR_RANGE range{};
				range.Init(type, 1, registerNum[(int)type]);

				registerNum[(int)type]++;
				rangeArray.emplace_back(range);
			}
			//�r���[�ŏ�����
			else
			{
				rootParameters.emplace_back();
				if (param.viewType == SRV)
				{
					auto type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
					rootParameters.back().InitAsShaderResourceView(registerNum[(int)type]);
					registerNum[(int)type]++;
				}
				else if (param.viewType == CBV)
				{
					auto type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					rootParameters.back().InitAsConstantBufferView(registerNum[(int)type]);
					registerNum[(int)type]++;
				}
				else if (param.viewType == UAV)
				{
					auto type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
					rootParameters.back().InitAsUnorderedAccessView(registerNum[(int)type]);
					registerNum[(int)type]++;
				}
				else
				{
					ASSERT_MSG("���[�g�p�����[�^����肭�����܂���ł���\n");
				}
			}
		}

		for (int i = 0; i < rangeArray.size(); ++i)
		{
			rootParameters.emplace_back();
			rootParameters.back().InitAsDescriptorTable(1, &rangeArray[i]);
		}

		// ���[�g�V�O�l�`���̐ݒ�
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_0(rootParameters.size(), &rootParameters[0], 1, &Sampler.sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> rootSigBlob;
		ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g
		// �o�[�W������������̃V���A���C�Y
		auto hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
		if (FAILED(hr))
		{
			//errorBlob����G���[���estring�^�ɃR�s�[
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				errstr.begin());
			errstr += '\n';
			//�G���[���e���o�̓E�B���h�E�ɕ\��
			OutputDebugStringA(errstr.c_str());
			ASSERT_MSG("���[�g�V�O�l�`���̃o�[�W������������̃V���A���C�Y���s\n");
		}

		// ���[�g�V�O�l�`���̐���
		hr = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

		//���[�g�V�O�l�`�������Ɏ��s
		if (FAILED(hr))ASSERT_MSG("���[�g�V�O�l�`���̐����Ɏ��s\n");
	}

	//�O���t�B�b�N�X�p�C�v���C���ݒ�Ƀ��[�g�V�O�l�`�����Z�b�g
	desc.pRootSignature = rootSignature.Get();

	desc.CS = CD3DX12_SHADER_BYTECODE(ComputeShader.Get());
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;

	ComPtr<ID3D12PipelineState>pipeline;
	hr = device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pipeline));
	if (FAILED(hr))ASSERT_MSG("�R���s���[�g�p�C�v���C�������Ɏ��s\n");

	return std::make_shared<ComputePipeline>(pipeline, rootSignature);
}

void D3D12App::SetViewPortsAndScissorRects(const Vec2<float>& Size)
{
	//�r���[�|�[�g�ݒ�
	commandList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, Size.x, Size.y));

	//�V�U�[��`�ݒ�
	commandList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, Size.x, Size.y));
}

void D3D12App::SetBackBufferRenderTarget()
{
	swapchain->GetBackBufferRenderTarget()->ChangeBarrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> BACK_BUFF_HANDLE = { swapchain->GetBackBufferRenderTarget()->AsRTV(commandList) };
	commandList->OMSetRenderTargets(BACK_BUFF_HANDLE.size(), &BACK_BUFF_HANDLE[0], FALSE, nullptr);
}

void D3D12App::SplitTextureBuffer(std::shared_ptr<TextureBuffer>* Array, const std::shared_ptr<TextureBuffer>& SorceTexBuffer, const int& AllNum, const Vec2<int>& SplitNum, const std::string& Name)
{
	//�f�B�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* heaps[] = { descHeapCBV_SRV_UAV->GetHeap().Get() };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);

	splitImgPipeline->SetPipeline(commandList);
	SorceTexBuffer->SetComputeDescriptorBuffer(commandList, SRV, 1);

	SplitImgConstData constData;

	//�����O�̃T�C�Y���L�^
	constData.splitSize = { static_cast<int>(SorceTexBuffer->GetDesc().Width) / SplitNum.x,static_cast<int>(SorceTexBuffer->GetDesc().Height) / SplitNum.y };

	for (int i = 0; i < AllNum; ++i)
	{
		if (splitImgConstBuff.size() < splitTexBuffCount + 1)
		{
			std::string name = "SplitImgConstBuff - " + std::to_string(i);
			splitImgConstBuff.emplace_back(GenerateConstantBuffer(sizeof(SplitImgConstData), 1, nullptr, name.c_str()));
		}

		//�`�����ݐ�p�̃e�N�X�`���o�b�t�@
		auto splitResult = GenerateTextureBuffer(constData.splitSize, SorceTexBuffer->GetDesc().Format, (Name + " - " + std::to_string(i)).c_str());

		splitResult->SetComputeDescriptorBuffer(commandList, UAV, 0);

		//splitImgConstBuff[i]->Mapping(&constData);
		//splitImgConstBuff[i]->SetComputeDescriptorBuffer(commandList, CBV, 2);
		splitImgConstBuff[splitTexBuffCount]->Mapping(&constData);
		splitImgConstBuff[splitTexBuffCount]->SetComputeDescriptorBuffer(commandList, CBV, 2);

		static const int THREAD_NUM = 8;
		const UINT threadX = constData.splitSize.x / THREAD_NUM;
		const UINT threadY = constData.splitSize.y / THREAD_NUM;
		commandList->Dispatch(threadX, threadY, 1);

		//�e�N�X�`���p�̃��\�[�X�o���A�ɕύX
		splitResult->ChangeBarrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		Array[i] = splitResult;

		constData.imgNum.x++;
		if (SplitNum.x <= constData.imgNum.x)
		{
			constData.imgNum.x = 0;
			constData.imgNum.y++;
		}

		splitTexBuffCount++;
	}
}

std::vector<std::shared_ptr<TextureBuffer>> D3D12App::SplitTextureBuffer(const std::shared_ptr<TextureBuffer>& SorceTexBuffer, const int& AllNum, const Vec2<int>& SplitNum, const std::string& Name)
{
	std::vector<std::shared_ptr<TextureBuffer>>result(AllNum);
	SplitTextureBuffer(&result[0], SorceTexBuffer, AllNum, SplitNum, Name);
	return result;
}