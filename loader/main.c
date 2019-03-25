#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(
	EFI_HANDLE imageHandle,
	EFI_SYSTEM_TABLE *systemTable
){
	InitializeLib(imageHandle, systemTable);
	Print(L"Hello UEFI World!\n");
	while(1);
	return EFI_SUCCESS;
}

