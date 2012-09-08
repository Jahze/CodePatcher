#ifndef PATCHMANAGER_H_
#define PATCHMANAGER_H_

#include <vector>

class Patch;
typedef std::vector<Patch*> PatchCollection;

class PatchManager {
public:
	~PatchManager();

	void AddPatch(Patch *pPatch);

	Patch *GetPatchByName(const std::string& name);
	PatchCollection& GetAllPatches();

	void UnloadAllPatches();

private:
	PatchCollection m_patches;
};

// FIXME: lame singleton
extern PatchManager g_PatchManager;

#endif
