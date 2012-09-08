#include "Patch.h"
#include "PatchManager.h"

PatchManager g_PatchManager;

PatchManager::~PatchManager() {
	for (PatchCollection::iterator iter = m_patches.begin(); iter != m_patches.end(); ++iter) {
		delete *iter;
	}
}

void PatchManager::AddPatch(Patch *pPatch) {
	m_patches.push_back(pPatch);
}

Patch* PatchManager::GetPatchByName(const std::string& name) {
	for (PatchCollection::iterator iter = m_patches.begin(); iter != m_patches.end(); ++iter) {
		if ((*iter)->GetName() == name)
			return *iter;
	}
	return NULL;
}

PatchCollection& PatchManager::GetAllPatches() {
	return m_patches;
}

void PatchManager::UnloadAllPatches() {
	for (PatchCollection::iterator iter = m_patches.begin(); iter != m_patches.end(); ++iter) {
		(*iter)->Unpatch();
	}
}
