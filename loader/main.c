#include <efi.h>
#include <efilib.h>

EFI_STATUS WaitForAnyKey(OUT EFI_INPUT_KEY *key){
	UINTN index;
	uefi_call_wrapper(
		BS->WaitForEvent,
		3,
		1,
		&(ST->ConIn->WaitForKey),
		&index
	);
	uefi_call_wrapper(
		ST->ConIn->ReadKeyStroke,
		2,
		ST->ConIn,
		key
	);
	return EFI_SUCCESS;
};

EFI_STATUS efi_main(
	EFI_HANDLE imageHandle,
	EFI_SYSTEM_TABLE *systemTable
){
	InitializeLib(imageHandle, systemTable);
	Print(L"FROS Loader Launched\n");

	EFI_STATUS ret;

	UINTN handleCount = 10;
	EFI_HANDLE *handleBuffer;
	ret=uefi_call_wrapper(
		BS->LocateHandleBuffer,
		5,
		ByProtocol,
		&gEfiSimpleFileSystemProtocolGuid,
		NULL,
		&handleCount,
		&handleBuffer
	);
	if(ret!=EFI_SUCCESS){
		return ret;
	}
	Print(L"%d esp(s) found\n", handleCount);

	for(UINTN i=0;i<handleCount;i++){
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *protocol;
		EFI_FILE_PROTOCOL *rootDirectory, *file;

		ret=uefi_call_wrapper(
			BS->HandleProtocol,
			3,
			handleBuffer[i],
			&gEfiSimpleFileSystemProtocolGuid,
			&protocol
		);
		if(ret!=EFI_SUCCESS){
			return ret;
		}
		Print(L"esp simple file system protocol at 0x%X\n", protocol);

		ret=uefi_call_wrapper(
			protocol->OpenVolume,
			2,
			protocol,
			&rootDirectory
		);
		if(ret!=EFI_SUCCESS){
			return ret;
		}
		Print(L"esp root dir file protocol at 0x%X\n", rootDirectory);

		ret=uefi_call_wrapper(
			rootDirectory->Open,
			5,
			rootDirectory,
			&file,
			L"fros\\kernel.elf",
			EFI_FILE_MODE_READ,
			EFI_FILE_READ_ONLY
		);
		if(ret!=EFI_SUCCESS){
			return ret;
		}
		Print(L"kernel file protocol at 0x%X\n", file);
	}

	uefi_call_wrapper(
		BS->FreePool,
		1,
		handleBuffer
	);

	EFI_INPUT_KEY key;
	WaitForAnyKey(&key);
	return EFI_SUCCESS;
}

