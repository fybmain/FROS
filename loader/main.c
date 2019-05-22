#include <efi.h>
#include <efilib.h>

const CHAR16 *EfiMemoryTypeString[] = {
	L"EfiReservedMemoryType",
	L"EfiLoaderCode",
	L"EfiLoaderData",
	L"EfiBootServicesCode",
	L"EfiBootServicesData",
	L"EfiRuntimeServicesCode",
	L"EfiRuntimeServicesData",
	L"EfiConventionalMemory",
	L"EfiUnusableMemory",
	L"EfiACPIReclaimMemory",
	L"EfiACPIMemoryNVS",
	L"EfiMemoryMappedIO",
	L"EfiMemoryMappedIOPortSpace",
	L"EfiPalCode",
	L"EfiPersistentMemory",

	L"Unknown",
};

EFI_STATUS WaitForAnyKey(IN EFI_INPUT_KEY *key){
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
		3,
		ST->ConIn,
		key
	);
	return EFI_SUCCESS;
};

EFI_STATUS KeyStrokeTest(){
	EFI_INPUT_KEY key;

	Print(L"Key Stroke Test\n");

	Print(L"Please hit a key...\n");
	WaitForAnyKey(&key);
	Print(
		L"Key: ScanCode:0x%x Unicode:0x%x\n",
		key.ScanCode,
		key.UnicodeChar
	);

	return EFI_SUCCESS;
}

EFI_STATUS GetMemoryMap(
	OUT UINTN *pMemoryMapSize,
	OUT EFI_MEMORY_DESCRIPTOR **ppMemoryMap,
	OUT UINTN *pMapKey,
	OUT UINTN *pDescriptorSize,
	OUT UINT32 *pDescriptorVersion
){
	EFI_STATUS status;

	*pMemoryMapSize = 0;
	status = uefi_call_wrapper(
		BS->GetMemoryMap,
		5,
		pMemoryMapSize,
		NULL,
		pMapKey,
		pDescriptorSize,
		pDescriptorVersion
	);
	if(status==EFI_BUFFER_TOO_SMALL){
		// *pMemoryMapSize will be updated by UEFI
	}else{
		EFI_ERROR(status);
		return status;
	}

	while(TRUE){
		*pMemoryMapSize += 100 * sizeof(EFI_MEMORY_DESCRIPTOR);

		status = uefi_call_wrapper(
			BS->AllocatePool,
			3,
			EfiLoaderData,
			*pMemoryMapSize,
			ppMemoryMap
		);
		if(EFI_ERROR(status)){
			return status;
		}

		status = uefi_call_wrapper(
			BS->GetMemoryMap,
			5,
			pMemoryMapSize,
			*ppMemoryMap,
			pMapKey,
			pDescriptorSize,
			pDescriptorVersion
		);
		if(status==EFI_SUCCESS){
			break;
		}else{
			uefi_call_wrapper(
				BS->FreePool,
				1,
				*ppMemoryMap
			);
			if(status==EFI_BUFFER_TOO_SMALL){
				// *pMemoryMapSize will be updated by UEFI
			}else{
				return status;
			}
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS PrintMemoryMap(){
	EFI_STATUS status;

	EFI_MEMORY_DESCRIPTOR *pMemoryMap;
	UINTN memoryMapSize, mapKey, descriptorSize;
	UINT32 descriptorVersion;

	status = GetMemoryMap(
		&memoryMapSize,
		&pMemoryMap,
		&mapKey,
		&descriptorSize,
		&descriptorVersion
	);
	if(EFI_ERROR(status)){
		return status;
	}

	UINTN memoryMapItemNum = memoryMapSize / descriptorSize;
	Print(L"MemoryMapItemNum=%lu\n", memoryMapItemNum);
	for(UINTN i=0;i<memoryMapItemNum;i++){
		EFI_MEMORY_DESCRIPTOR *cur = (EFI_MEMORY_DESCRIPTOR*)(((UINT8*)pMemoryMap)+descriptorSize*i);
		if(i!=0){
			EFI_INPUT_KEY key;
			WaitForAnyKey(&key);
		}

		Print(L"MemoryMap[%lu]:\n", i);
		const CHAR16 *typeString;
		if((cur->Type)>EfiMaxMemoryType){
			typeString = EfiMemoryTypeString[EfiMaxMemoryType];
		}else{
			typeString = EfiMemoryTypeString[cur->Type];
		}
		Print(L"\tType: %s\n", typeString);
		Print(L"\tPhysicalStart: 0x%016lX\n", cur->PhysicalStart);
		Print(L"\tVirtualStart: 0x%016lX\n", cur->VirtualStart);
		Print(L"\tNumberOfPages: %lu\n", cur->NumberOfPages);
		Print(L"\tAttribute: 0x%016lX\n", cur->Attribute);
		Print(L"\n");
	}

	BS->FreePool(pMemoryMap);
	return EFI_SUCCESS;
}

EFI_STATUS efi_main(
	EFI_HANDLE imageHandle,
	EFI_SYSTEM_TABLE *systemTable
){
	EFI_STATUS status;
	EFI_INPUT_KEY key;

	InitializeLib(imageHandle, systemTable);
	Print(L"Hello UEFI World!\n");

	status = KeyStrokeTest();
	if(EFI_ERROR(status)){
		return status;
	}

	status = PrintMemoryMap();
	if(EFI_ERROR(status)){
		return status;
	}

	Print(L"Hit any key to exit\n");
	WaitForAnyKey(&key);

	return EFI_SUCCESS;
}

