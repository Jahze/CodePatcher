#include "extension.h"
#include "Patch.h"
#include "PatchManager.h"
#include "PatchParser.h"

namespace {
	bool IsHexCharacter(unsigned char hex) {
		if (hex >= '0' && hex <= '9')
			return true;

		if (hex >= 'A' && hex <= 'F')
			return true;

		if (hex >= 'a' && hex <= 'f')
			return true;

		return false;
	}

	unsigned HexValue(unsigned char hex) {
		if (hex >= '0' && hex <= '9')
			return hex - '0';

		if (hex >= 'A' && hex <= 'F')
			return hex - 'A' + 10;

		if (hex >= 'a' && hex <= 'f')
			return hex - 'a' + 10;

		return 0;
	}
}

SourceMod::SMCResult PatchParser::ReadSMC_NewSection(const SourceMod::SMCStates *states, const char *name) {
	m_currentPatch.m_name = name;
	return SourceMod::SMCResult_Continue;
}

SourceMod::SMCResult PatchParser::ReadSMC_KeyValue(const SourceMod::SMCStates *states, const char *key, const char *value) {
	std::string strKey = key;
	if (strKey == "desc" || strKey == "description") {
		m_currentPatch.m_description = value;
	}
	else if (strKey == "sig" || strKey == "signature") {
		m_currentPatch.m_signature = value;
	}
#ifdef _WINDOWS
	else if (strKey == "windows") {
#else
	else if (strKey == "linux") {
#endif
		// FIXME: fugly
		m_currentPatch.m_length = 0;

		unsigned length = strlen(value);
		for (unsigned i = 0; i < length; ++i) {
			if (value[i] == '\\') {
				if (i + 3 < length
				&& value[i+1] == 'x'
				&& ::IsHexCharacter(value[i+2])
				&& ::IsHexCharacter(value[i+3])) {
					unsigned val = (::HexValue(value[i+2]) << 4);
					val += ::HexValue(value[i+3]);
					m_currentPatch.m_bytes[m_currentPatch.m_length++] = val;
					i += 3;
					continue;
				}
			}

			m_currentPatch.m_bytes[m_currentPatch.m_length++] = value[i];
		}

		for (unsigned i = 0; i < m_currentPatch.m_length; ++i) {
			g_pSM->LogMessage(myself, "0x%x", m_currentPatch.m_bytes[i]);
		}
	}
#ifdef _WINDOWS
	else if (strKey == "linux") {
#else
	else if (strKey == "windows") {
#endif
	}
	else {
		g_pSM->LogError(myself, "Unknown key \"%s\" for patch at line %d", key, states->line);
	}

	return SourceMod::SMCResult_Continue;
}

SourceMod::SMCResult PatchParser::ReadSMC_LeavingSection(const SourceMod::SMCStates *states) {
	m_patches.push_back(m_currentPatch);

	m_currentPatch.m_name.clear();
	m_currentPatch.m_description.clear();
	m_currentPatch.m_signature.clear();
	m_currentPatch.m_length = 0;
	return SourceMod::SMCResult_Continue;
}

void PatchParser::AddPatches(SourceMod::IGameConfig *pCfg) {
	for (PatchEntryCollection::iterator iter = m_patches.begin(); iter != m_patches.end(); ++iter) {
		void *pAddress;
		if (! pCfg->GetMemSig(iter->m_signature.c_str(), &pAddress)) {
			g_pSM->LogError(myself, "Failed to find signature for patch \"%s\"", iter->m_name.c_str());
			continue;
		}

		int offset;
		if (! pCfg->GetOffset(iter->m_signature.c_str(), &offset)) {
			g_pSM->LogError(myself, "Failed to find offset for patch \"%s\"", iter->m_name.c_str());
			continue;
		}

		pAddress = reinterpret_cast<unsigned char*>(pAddress) + offset;

		Patch *patch = new Patch(iter->m_name, iter->m_description, pAddress, iter->m_bytes, iter->m_length);
		g_PatchManager.AddPatch(patch);
	}
}
