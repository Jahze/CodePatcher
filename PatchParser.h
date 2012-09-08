#ifndef PATCHPARSER_H_
#define PATCHPARSER_H_

#include <IGameConfigs.h>
#include <ITextParsers.h>
#include <string>
#include <vector>

#define MAX_PATCH_LENGTH	512

struct PatchEntry {
	std::string m_name;
	std::string m_description;
	std::string m_signature;
	char m_bytes[MAX_PATCH_LENGTH];
	unsigned m_length;
};

typedef std::vector<PatchEntry> PatchEntryCollection;

class PatchParser : public SourceMod::ITextListener_SMC {
public:
	SourceMod::SMCResult ReadSMC_NewSection(const SourceMod::SMCStates *states, const char *name);
	SourceMod::SMCResult ReadSMC_KeyValue(const SourceMod::SMCStates *states, const char *key, const char *value);
	SourceMod::SMCResult ReadSMC_LeavingSection(const SourceMod::SMCStates *states);

	void AddPatches(SourceMod::IGameConfig *pCfg);

private:
	PatchEntry m_currentPatch;
	PatchEntryCollection m_patches;
};

#endif
