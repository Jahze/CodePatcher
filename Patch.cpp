#ifdef _WINDOWS
	#include <Windows.h>
#else
	#include <dlfcn.h>
	#include <sys/mman.h>
#endif

#include "extension.h"
#include "Patch.h"

#define PAGE_SIZE	4096
#define ALIGN(ar)	((long)ar & ~(PAGE_SIZE-1))

#ifndef _WINDOWS
	#define PAGE_EXECUTE_READWRITE	PROT_READ|PROT_WRITE|PROT_EXEC
#endif

namespace {
	void AllowMemWrite(void *addr, int length) {
		void *p = (void*)ALIGN(addr);
#ifdef _WINDOWS
		DWORD oldProt;
		if (! VirtualProtect(addr, length, PAGE_EXECUTE_READWRITE, &oldProt))
			g_pSM->LogError(myself, "Failed to change: 0x%08x", GetLastError());
		else
			g_pSM->LogMessage(myself, "Changed permissions");
#else
		mprotect(p, sysconf(_SC_PAGESIZE), PAGE_EXECUTE_READWRITE);
#endif
}

	void *GetAddress(const char *symbol, int length) {
		if (g_SMAPI == NULL) {
			g_pSM->LogError(myself, "GetAddress(): No SMAPI");
			return 0;
		}

		void *addrInBase = (void*)g_SMAPI->GetServerFactory(false);
		if (addrInBase == NULL)
			return 0;

#ifdef _WINDOWS
		return memutils->FindPattern(addrInBase, symbol, length);
#else
		Dl_info info;
		if (dladdr(addrInBase, &info) == 0)
			return 0;

		void *handle = dlopen(info.dli_fname, RTLD_NOW);
		if (!handle)
			return 0;

		handle = memutils->ResolveSymbol(handle, symbol);
		dlclose(handle);
		return handle;
#endif
	}
}

Patch::Patch(const std::string& name, const std::string& desc, void *pAddress, const char *bytes, unsigned length)
	: m_name(name)
	, m_description(desc)
	, m_pAddress(pAddress)
	, m_pOldBytes(NULL)
	, m_length(length)
	, m_bPatched(false)
{
	assert(length != 0);
	m_pPatchBytes = new char[length];
	memcpy(m_pPatchBytes, bytes, length);
}

Patch::~Patch() {
	if (IsPatched())
		Unpatch();

	delete[] m_pPatchBytes;
	if (m_pOldBytes)
		delete[] m_pOldBytes;
}

std::string Patch::GetName() const {
	return m_name;
}

std::string Patch::GetDescription() const {
	return m_description;
}

void* Patch::GetAddress() const {
	return m_pAddress;
}

unsigned Patch::GetLength() const {
	return m_length;
}

bool Patch::IsPatched() const {
	return m_bPatched;
}

bool Patch::ApplyPatch() {
	if (IsPatched())
		return true;

	if (m_pOldBytes == NULL) {
		m_pOldBytes = new char[m_length];
		memcpy(m_pOldBytes, m_pAddress, m_length);
	}

	::AllowMemWrite(m_pAddress, m_length);
	memcpy(m_pAddress, m_pPatchBytes, m_length);

	m_bPatched = true;
	return true;
}

void Patch::Unpatch() {
	if (! IsPatched())
		return;

	memcpy(m_pAddress, m_pOldBytes, m_length);
	m_bPatched = false;
}
